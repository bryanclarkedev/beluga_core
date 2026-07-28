#include "beluga_machine.h"
#include "beluga_comms.h"
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
    */
    
    bool machine::read_config()
    {
        bool config1_ok = beluga_core::device::read_config();
        bool config2_ok = initialise_subdevices();
        return (config1_ok && config2_ok);
        
    }

    /*
    initialise_subdevices calls the subdevices that this machine contains and initialises them each in turn.
    Since it might contain machines, initialise_subdevices could be called inside the subdevices i.e. it effectively
    crawls the subdevice tree of the whole system.
    example config:
    [machine_name1]
    enable_serial_debug = true
    enabled = true
    subdevice_names = dname1,dname2

    [dname1]
    device_type = type1
    etc

    [dname2]
    device_type = type2
    ...
    subdevice_names = dname3
    etc

    [dname3]
    device_type = type3

    etc
    */
    bool machine::initialise_subdevices()
    {
        //std::string name_config_key = "subdevice_names";
        std::vector<std::string> raw_subdevice_names;

        bool names_ok = _ini_ptr->get_config_list_field(_config_file_section, beluga_utils::subdevice_names_key, raw_subdevice_names);
        if(! names_ok)
        {
            //_initialisation_error = true;
            //No subdevices.
            return true;
        }

        //Now go to the config for each machine, and get its machine type
        bool config_ok = false;
        std::string device_type_str;
        //std::string config_key = "device_type";
        for(auto iter = raw_subdevice_names.begin(); iter != raw_subdevice_names.end(); iter++)
        {
            std::string this_subdevice_name = *iter;
            Serial.print("Attempting to initialise device ");
            Serial.println(this_subdevice_name.c_str());
            //Use device name as config_file_section
            config_ok = _ini_ptr->get_config_value(this_subdevice_name, beluga_utils::device_type_key, &device_type_str );

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
            //----config is good. Now we need to add to the machine----
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
    Can't remember why I instantiated the factory as a shared ptr rather than as a straight variable declaration.
    TODO: Change it to a variable.
    Also need to rejigger it somehow so that when I add new classes the factory can be updated easily.
    */
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
        bool device_ok = factory_ptr->try_create(subdevice_type, device_ptr);
        if(! device_ok)
        {
            Serial.print("Error creating subdevice ");
            Serial.print(subdevice_name.c_str());
            Serial.print(" type ");
            Serial.println(subdevice_type.c_str());
            Serial.println(">>>>Check the subdevice type label is not misspelt!");
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
            Serial.print(subdevice_type.c_str());
            Serial.print(" (is comms: ");
            Serial.print(device_ptr->get_is_comms());
            Serial.println(")");

            if(device_ptr->get_is_comms()){
                _comms_names.push_back(subdevice_name);
            }
            return true;
        }else{
            Serial.print("Error initialising subdevice ");
            Serial.print(subdevice_name.c_str());
            Serial.print(" type ");
            Serial.println(subdevice_type.c_str());
            return false;
        }
        
    }
    
    /*
    Re-implement this function to accomodate extensions e.g. additional classes supported by an extended_factory that inherits from beluga_core::factory
    */
    bool machine::get_factory(std::shared_ptr<beluga_core::factory>  this_factory)
    {
        this_factory = std::make_shared<beluga_core::factory>();
        return true;
    }



    bool machine::run(void * p)
    {
        //Run comms rx
        for(auto comms_iter = _comms_names.begin(); comms_iter != _comms_names.end(); comms_iter++)
        {
            std::shared_ptr<beluga_core::comms> comms_ptr = std::static_pointer_cast<beluga_core::comms>(_subdevices[*comms_iter]);
            comms_ptr->run_rx();
        }
        //Run devices (including comms)
        for(auto subdevice_iter = _subdevices.begin(); subdevice_iter != _subdevices.end(); subdevice_iter++)
        {
            subdevice_iter->second->run(p);
        }

        //Run comms tx
        for(auto comms_iter = _comms_names.begin(); comms_iter != _comms_names.end(); comms_iter++)
        {
            std::shared_ptr<beluga_core::comms> comms_ptr = std::static_pointer_cast<beluga_core::comms>(_subdevices[*comms_iter]);
            comms_ptr->run_tx();
        }



        
        return true;
    }
}