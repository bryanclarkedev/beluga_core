#include "beluga_machinery.h"

namespace beluga_core
{

    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    void swap(machinery& first, machinery& second) // nothrow
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        swap(static_cast<machine&>(first), static_cast<machine&>(second));

        swap(first._comms_map, second._comms_map);  
        swap(first._comms_names, second._comms_names);
        swap(first._comms_types, second._comms_types);

        swap(first._primary_rx_list, second._primary_rx_list);  
        swap(first._primary_tx_list, second._primary_tx_list);  

    }

    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    machinery& machinery::operator=(machinery other) 
    {
        swap(*this, other); 
        return *this;
    }
    /*
    example config:
    [machinery_name1]
    enable_serial_debug = true
    enabled = true
    subdevice_names = dname1,dname2
    comms_names = cname1,cname2
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

    [cname1]
    device_type = typeX
    etc

    [cname2]
    device_type = typeY
    etc
    */
    bool machinery::read_config()
    {
        bool read_ok = beluga_core::machine::read_config();
        bool comms_ok = read_config_comms();
        
        std::string rx_mail_topic_key("rx_mail_topic");
        bool mail_rx_topic_ok = _ini_ptr->get_config_value(_config_file_section, rx_mail_topic_key, &_rx_mail_topic);
        if(! mail_rx_topic_ok)
        {
            _rx_mail_topic = _config_file_section ;
        }
        std::string tx_mail_topic_key("tx_mail_topic");
        bool mail_tx_topic_ok = _ini_ptr->get_config_value(_config_file_section, tx_mail_topic_key, &_tx_mail_topic);
        if(! mail_tx_topic_ok)
        {
            _tx_mail_topic = _config_file_section ;
        }
        /*
        std::string parser_config_name_key("parser_config_section");
        bool parser_config_ok = _ini_ptr->get_config_value(_config_file_section, parser_config_name_key, &_parser_config_section);
        if(! parser_config_ok)
        {
            _parser_config_section = "globals" ;
        }
        */
        /*
        bool parser_ok = _parser.initialise(_ini_ptr, _parser_config_section);
        if(! parser_ok)
        {
            error_loop_forever("machinery error: could not initialise the string parser");
        }
        */
        return read_ok;
    }

    bool machinery::read_config_comms()
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
            return true; //No comms is an acceptable option
        }

        //Now go to the config for each device, and get its device type
        bool config_ok = false;
        std::string device_type_str;
        for(auto iter = raw_comms_names.begin(); iter != raw_comms_names.end(); iter++)
        {
            std::string this_comms_device_name = *iter;

            //Use sensor name as config_file_section
            config_ok = _ini_ptr->get_config_value(this_comms_device_name, beluga_utils::device_type_key, &device_type_str );

            if(! config_ok)
            {
                beluga_utils::debug_print("Error reading comms device type config for section ",  false);
                beluga_utils::debug_print(this_comms_device_name);
                _initialisation_error = true;
                return false;
            }


            //----config is good. Now we need to add the comms----
            bool this_init_ok = initialise_comms_subdevice(this_comms_device_name, device_type_str);
            if(! this_init_ok)
            {
                _initialisation_error = true;
                return false;
            }

            #if 0
            std::shared_ptr<beluga_core::device> comms_device_ptr;
            bool device_ok = factory_ptr->try_create(device_type_str, comms_device_ptr);
            if(device_ok)
            {
                _comms_names.push_back(this_subdevice_name);
                _comms_types.push_back(device_type_str);
                std::shared_ptr<beluga_core::comms> comms_ptr;
                comms_ptr = std::static_pointer_cast<beluga_core::comms>(comms_device_ptr);
                _comms_map[this_subdevice_name] = comms_ptr;
                Serial.println("Trying to init comms from machinery...");
                Serial.println("-----THE ERROR IS SOMEWHERE BELOW HERE-------");

                _comms_map[this_subdevice_name]->initialise(_ini_ptr, this_subdevice_name);
                continue;
            }else{
                Serial.print("Error initialising comms subdevice: ");
                Serial.println(this_subdevice_name.c_str());
                Serial.println(_config_file_section.c_str());
                _initialisation_error = true;
                return false;
            }
            #endif
        }
        return true;
    }

/*
    Can't remember why I instantiated the factory as a shared ptr rather than as a straight variable declaration.
    TODO: Change it to a variable.
    Also need to rejigger it somehow so that when I add new classes the factory can be updated easily.
    */
    bool machinery::initialise_comms_subdevice(std::string comms_subdevice_name, std::string comms_subdevice_type)
    {
        std::shared_ptr<beluga_core::factory> factory_ptr;
        bool factory_ok = get_factory(factory_ptr);
        if(! factory_ok)
        {
            Serial.println("Error initialising factory.");
            _initialisation_error = true;
            return false;
        }

        std::shared_ptr<beluga_core::device> comms_device_ptr;
        bool device_ok = factory_ptr->try_create(comms_subdevice_type, comms_device_ptr);
        if(! device_ok)
        {
            Serial.print("Error creating subdevice ");
            Serial.print(comms_subdevice_name.c_str());
            Serial.print(" type ");
            Serial.println(comms_subdevice_type.c_str());
            return false;
        }

        _comms_names.push_back(comms_subdevice_name);
        _comms_types.push_back(comms_subdevice_type);

        //Cast from a shared_ptr<device> to shared_ptr<comms>
        std::shared_ptr<beluga_core::comms> comms_ptr;
        comms_ptr = std::static_pointer_cast<beluga_core::comms>(comms_device_ptr);
        comms_ptr->set_parent(std::shared_ptr<beluga_core::device>(this));

        bool this_init_ok = comms_ptr->initialise(_ini_ptr, comms_subdevice_name);
        if(this_init_ok)
        {
            _comms_map[comms_subdevice_name] = comms_ptr;
            return true; 
        }else{
            Serial.print("Error initialising comms subdevice ");
            Serial.print(comms_subdevice_name.c_str());
            Serial.print(" type ");
            Serial.println(comms_subdevice_type.c_str());
            return false;
        }
        
    }
    

   /*
   Pre-existing comms can be passed in.
   */
    bool machinery::set_comms(std::map<std::string, std::shared_ptr<beluga_core::comms> > & comms_map, std::vector<std::string> comms_types)
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



    bool machinery::add_rx(std::list<std::string> & rx, bool overwrite_not_append )
    {
        if(overwrite_not_append)
        {
            _primary_rx_list.clear();//Overwrite
        }
        _primary_rx_list = rx;

        return true;
    }

    bool machinery::get_tx(std::list<std::string> & tx, bool overwrite_not_append)
    {
        if(overwrite_not_append)
        {
            tx.clear();//Overwrite
        }
        tx = _primary_tx_list;
        _primary_tx_list.clear();
        return true;
    }


    bool machinery::run(void * p)
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