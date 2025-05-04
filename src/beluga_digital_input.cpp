#include "beluga_digital_input.h"
namespace beluga_core
{
    //copy-and-sep for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    void swap(digital_input& first, digital_input& second) // nothrow
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        swap(static_cast<mechanism<bool> &>(first), static_cast<mechanism<bool> &>(second));

        swap(first._pin, second._pin);   
    }

    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    digital_input& digital_input::operator=(digital_input other) 
    {
        swap(*this, other); 
        return *this;
    }

    bool digital_input::read_config()
    {
        bool config_ok = false;
        std::string config_val;
        config_ok = _ini_ptr->get_config_value(_config_file_section, "enable_serial_debug", &config_val );
        if(config_ok)
        {
            set_serial_debug_enabled(config_val);
        }    

        _pin.set_pin_direction(INPUT);
        _pin.initialise(_ini_ptr, _config_file_section);
        _pin.configure();

        add_new_value();
        
        return true;
    }

    bool digital_input::run(void * p )
    {
        if (!_pin.get_is_configured())
        {
            return false;
        }
        bool digital_val;
        bool read_ok = _pin.digital_read(digital_val);
        if(read_ok)
        {
            bool stored_ok = set_value(digital_val); // _states[_config_file_section].set_int16_state(digital_val);
            if (! stored_ok)
            {
                Serial.println("Could not store digital input state!");
            }
        }
        return true;
    }

}