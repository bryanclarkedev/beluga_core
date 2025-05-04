#include "beluga_device.h"

namespace beluga_core
{


    /*!
    \brief Base class 
    \author Bryan Clarke
    \date gotta check
    \details Given a config file path and config section key, initialises this device.
    */
    bool device::initialise(std::string config_file_path, std::string config_section)
    {
        //Serial.println("Beluga device init");
        _config_file_path = config_file_path;
        _config_file_section = config_section;
        _ini_ptr = std::make_shared<beluga_utils::ini_reader>(config_file_path);
        _ini_ptr->initialise();

        bool config_ok = read_config();
        _ini_ptr.reset(); //Delete the shared_ptr for the config
        return config_ok;
    }

    //Initialise using a given ini reader
    bool device::initialise(std::shared_ptr<beluga_utils::ini_reader> ini, std::string config_section)
    {
        _ini_ptr = ini;
        _ini_ptr->initialise();

        _config_file_section = config_section;
        return read_config();
    }

    bool device::read_config()
    {
        bool config_ok = false;
        std::string config_val;
        config_ok = _ini_ptr->get_config_value(_config_file_section, "enable_serial_debug", &config_val );
        if(config_ok)
        {
            set_serial_debug_enabled(config_val);
        }  

        bool enable_ok = false;
        enable_ok = _ini_ptr->get_config_value(_config_file_section, "enabled", &config_val );
        if(config_ok)
        {
            set_enabled(config_val);
        }  

        return true;
    }

    void device::set_enabled(std::string s)
    {
        set_enabled(beluga_utils::string_to_bool(s));    
        return;
    }

    void device::set_enabled(bool b)
    {
        _enabled = b;
        return;
    }

    void device::set_serial_debug_enabled(std::string s)
    {
        set_serial_debug_enabled(beluga_utils::string_to_bool(s));    
        return;
    }

    void device::set_serial_debug_enabled(bool b)
    {
        _serial_debug_enabled = b;
        return;
    }
}