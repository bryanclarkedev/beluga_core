#include "beluga_digital_output.h"
namespace beluga_core
{
    //copy-and-sep for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    void swap(digital_output& first, digital_output& second) // nothrow
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        swap(static_cast<mechanism<bool> &>(first), static_cast<mechanism<bool> &>(second));

        swap(first._pin, second._pin);   
    }

    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    digital_output& digital_output::operator=(digital_output other) 
    {
        swap(*this, other); 
        return *this;
    }

    bool digital_output::read_config()
    {
        //bool ini_ok = _ini_ptr->initialise(); //Will always be true, else the ini.initialise() will be in an endless loop of failure.

        bool config_ok = false;
        std::string config_val;
        config_ok = _ini_ptr->get_config_value(_config_file_section, "enable_serial_debug", &config_val );
        if(config_ok)
        {
            set_serial_debug_enabled(config_val);
        }    

        _pin.set_pin_direction(OUTPUT);
        _pin.initialise(_ini_ptr, _config_file_section);
        _pin.configure();

        add_new_value();
        
        return true;
    }



    bool digital_output::run(void * p )
    {
        if (!_pin.get_is_configured())
        {
            Serial.println("Digital output not configured");
            return false;
        }
        //We only set a new value when the value has changed
        if(get_value_changed()) //Will auto-clear the change flag.
        {
            bool sp;
            bool got_value = get_value(sp);
            if(got_value)
            {
                _pin.digital_write(sp);
            }else{
                Serial.println("ERROR: Could not get setpoint!");
            }
        }
        return true;
    }

    #if 0
    bool digital_output::get_state(int16_t & return_val)
    {
        return _state.get_state(return_val);
    }

    bool digital_output::set_state(int16_t s)
    {
        return _states[_config_file_section].set_int16_state(s);
    }
    #endif
}