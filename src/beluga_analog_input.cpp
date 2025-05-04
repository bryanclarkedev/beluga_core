#include "beluga_analog_input.h"

namespace beluga_core
{
    //copy-and-sep for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    void swap(analog_input& first, analog_input& second) // nothrow
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        swap(static_cast<mechanism<int16_t> &>(first), static_cast<mechanism<int16_t> &>(second));

        swap(first._pin, second._pin);   
    }

    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    analog_input& analog_input::operator=(analog_input other) 
    {
        swap(*this, other); 
        return *this;
    }

    bool analog_input::read_config()
    {

        _pin.set_pin_direction(INPUT);//Hard code because it's an input
        _pin.initialise(_ini_ptr, _config_file_section);
        _pin.configure();

        add_new_value();

        return true;
    }

    bool analog_input::run(void * p )
    {
        if (!_pin.get_is_configured())
        {
            return false;
        }
        int16_t analog_val;
        bool read_ok = _pin.analog_read(analog_val);
        if(read_ok)
        {
            bool stored_ok = set_value(analog_val);// [_config_file_section].set_int16_state(analog_val);
            if (! stored_ok)
            {
                Serial.println("Could not store analog input state!");
            }
        }
        return true;
    }

}