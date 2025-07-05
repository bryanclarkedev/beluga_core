#include "beluga_machine.h"

namespace beluga_core
{

    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    void swap(machine& first, machine& second) // nothrow
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        swap(static_cast<device&>(first), static_cast<device&>(second));

        swap(first._subdevices, second._subdevices);  
        swap(first._subdevice_names, second._subdevice_names);
        swap(first._subdevice_types, second._subdevice_types);

        swap(first._comms_map, second._comms_map);  
        swap(first._comms_names, second._comms_names);
        swap(first._comms_types, second._comms_types);

    }

    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    machine& machine::operator=(machine other) 
    {
        swap(*this, other); 
        return *this;
    }

    /*
    This may need to be re-implemented for inheriting classes based on their requirements
    The implementation here may be useful (especially the initialisation of sub-machines)
    This can be called in an inheriting class via:

    inheriting_class::initialise(std::string config_file_path, std::string config_file_section)
    {
        //other code here
        machine::initialise(config_file_path, config_file_section);
        // other code here
    }
    */
   
    /*
    It is recommended that all objects inheriting from beluga_machine re-implement read_config
    They should start with:
    beluga_machine::read_config(); //Calls the base config, which calls initialise_subdevices


    config variables:
    [device_name1]
    enable_serial_debug = true
    enabled = true
    subdevice_names = name1,name2
    comms_names = cname1,cname2

    [name1]
    device_type = type1
    etc

    [name2]
    device_type = type2
    etc

    [cname1]
    device_type = type3
    etc

    [cname2]
    device_type = type4
    etc
    */
    bool machine::read_config()
    {
        bool config1_ok = beluga_core::device::read_config();
        bool config2_ok = initialise_subdevices();
        bool config3_ok = read_config_comms();
        return (config1_ok && config2_ok && config3_ok);
        
    }

    
    bool machine::read_config_comms()
    {
        //beluga_extensions::factory this_factory;
        std::shared_ptr<beluga_core::factory> factory_ptr;
        bool factory_ok = get_factory(factory_ptr);
        if(! factory_ok)
        {
            _initialisation_error = true;
            return false;
        }

        std::string name_config_key = "comms_names";
        
        std::vector<std::string> raw_comms_names;
        bool names_ok = _ini_ptr->get_config_list_field(_config_file_section, name_config_key, raw_comms_names);
        if(! names_ok)
        {
            Serial.println("No comms devices present");
            return false;
        }

        //Now go to the config for each device, and get its device type
        bool config_ok = false;
        std::string device_type_str;
        std::string config_key = "device_type";
        for(auto iter = raw_comms_names.begin(); iter != raw_comms_names.end(); iter++)
        {
            std::string this_subdevice_name = *iter;
            Serial.print("Attempting to initialise comms device ");
            Serial.println(this_subdevice_name.c_str());
            //Use sensor name as config_file_section
            config_ok = _ini_ptr->get_config_value(this_subdevice_name, config_key, &device_type_str );

            if(! config_ok)
            {
                Serial.print("Error reading comms device type config for section ");
                Serial.println(this_subdevice_name.c_str());
                _initialisation_error = false;
                return false;
            }
            //----config is good. Now we need to add the comms----
            std::shared_ptr<beluga_core::device> device_ptr;
            bool device_ok = factory_ptr->try_create(device_type_str, device_ptr);
            if(device_ok)
            {
                _comms_names.push_back(this_subdevice_name);
                _comms_types.push_back(device_type_str);
                std::shared_ptr<beluga_core::comms> comms_ptr;
                comms_ptr = std::static_pointer_cast<beluga_core::comms>(device_ptr);
                _comms_map[this_subdevice_name] = comms_ptr;
                _comms_map[this_subdevice_name]->initialise(_ini_ptr, this_subdevice_name);
                continue;
            }else{
                Serial.print("Error initialising comms subdevice: ");
                Serial.println(this_subdevice_name.c_str());
                Serial.println(_config_file_section.c_str());
                _initialisation_error = true;
                return false;
            }
        }
        return true;
    }

    /*
    Re-implement this function to accomodate extensions e.g. additional classes supported by an extended_factory that inherits from beluga_core::factory
    */
    bool machine::get_factory(std::shared_ptr<beluga_core::factory>  this_factory)
    {
        this_factory = std::make_shared<beluga_core::factory>();
        return true;
    }


    bool machine::initialise_subdevice(std::string subdevice_name, std::string subdevice_type)
    {
        std::shared_ptr<beluga_core::factory> factory_ptr;
        bool factory_ok = get_factory(factory_ptr);
        if(! factory_ok)
        {
            Serial.println("Error initialising factory.");
            _initialisation_error = true;
            return false;
        }

        std::shared_ptr<beluga_core::device> device_ptr;
        bool machine_ok = factory_ptr->try_create(subdevice_type, device_ptr);
        if(! machine_ok)
        {
            Serial.print("Error creating subdevice ");
            Serial.print(subdevice_name.c_str());
            Serial.print(" type ");
            Serial.println(subdevice_type.c_str());
            return false;
        }
        device_ptr->set_parent(std::shared_ptr<beluga_core::device>(this));

        Serial.print("Created subdevice ");
        Serial.print(subdevice_name.c_str());
        Serial.print(" type ");
        Serial.println(subdevice_type.c_str());
        _subdevice_names.push_back(subdevice_name);
        _subdevice_types.push_back(subdevice_type);
        _subdevices[subdevice_name] = device_ptr;
        bool this_init_ok = _subdevices[subdevice_name]->initialise(_ini_ptr, subdevice_name);
        if(this_init_ok)
        {
            Serial.print("Initialised subdevice ");
            Serial.print(subdevice_name.c_str());
            Serial.print(" type ");
            Serial.println(subdevice_type.c_str());
            return true;
        }else{
            Serial.print("Error initialising subdevice ");
            Serial.print(subdevice_name.c_str());
            Serial.print(" type ");
            Serial.println(subdevice_type.c_str());
            return false;
        }
        
    }

    bool machine::initialise_subdevices()
    {
        std::string name_config_key = "subdevice_names";
        std::vector<std::string> raw_subdevice_names;

        bool names_ok = _ini_ptr->get_config_list_field(_config_file_section, name_config_key, raw_subdevice_names);
        if(! names_ok)
        {
            _initialisation_error = true;
            return false;
        }

        //Now go to the config for each machine, and get its machine type
        bool config_ok = false;
        std::string device_type_str;
        std::string config_key = "device_type";
        for(auto iter = raw_subdevice_names.begin(); iter != raw_subdevice_names.end(); iter++)
        {
            std::string this_subdevice_name = *iter;
            Serial.print("Attempting to initialise device ");
            Serial.println(this_subdevice_name.c_str());
            //Use sensor name as config_file_section
            config_ok = _ini_ptr->get_config_value(this_subdevice_name, config_key, &device_type_str );

            if(! config_ok)
            {
                Serial.print("Error reading device type config for section ");
                Serial.println(this_subdevice_name.c_str());
                _initialisation_error = true;
                return false;
            }else{
                Serial.print("Got device type config for section ");
                Serial.println(this_subdevice_name.c_str());
            }
            //----config is good. Now we need to add the machine----
            bool this_init_ok = initialise_subdevice(this_subdevice_name, device_type_str);
            if(! this_init_ok)
            {
                _initialisation_error = true;
                return false;
            }
        }
        return true;
    }

   /*
   Pre-existing comms can be passed in.
   */
    bool machine::set_comms(std::map<std::string, std::shared_ptr<beluga_core::comms> > & comms_map, std::vector<std::string> comms_types)
    {
        auto iter1 = comms_map.begin();
        auto iter2 = comms_types.begin();
        for( ; iter1 != comms_map.end() && iter2 != comms_types.end(); iter1++, iter2++)
        {
            std::string this_name = iter1->first;
            _comms_names.push_back(this_name);
            _comms_map[this_name] = iter1->second;
            _comms_types.push_back(*iter2);
        }
        return true;
    }

    bool machine::run(void * p)
    {
        //Run comms rx
        for(auto comms_iter = _comms_map.begin(); comms_iter != _comms_map.end(); comms_iter++)
        {
            comms_iter->second->run_rx();
        }
        //Run devices
        for(auto subdevice_iter = _subdevices.begin(); subdevice_iter != _subdevices.end(); subdevice_iter++)
        {
            subdevice_iter->second->run(p);
        }
        //Comms are devices and so have a run() also
        for(auto comms_iter = _comms_map.begin(); comms_iter != _comms_map.end(); comms_iter++)
        {
            comms_iter->second->run();
        }
        //Run comms tx
        for(auto comms_iter = _comms_map.begin(); comms_iter != _comms_map.end(); comms_iter++)
        {
            comms_iter->second->run_tx();
        }
        return true;
    }
}