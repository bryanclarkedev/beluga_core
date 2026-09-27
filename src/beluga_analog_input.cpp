#include "beluga_analog_input.h"


#include "beluga_debug.h"
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
        bool config_ok = false;

        config_ok = device::read_config();
        if(! config_ok)
        {
            beluga_utils::debug_print_loop_forever("Could not read GPIO config...");
        }

        
        _pin.set_pin_direction(INPUT);
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
            bool stored_ok = set_value(analog_val); // _states[_config_file_section].set_int16_state(digital_val);
            if (! stored_ok)
            {
                Serial.println("Could not store analog input state!");
            }
        }
        return true;
        
    }

}

#if 0

#include "Beluga_Analog_Input.h"


//copy-and-sep for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
void swap(Beluga_Analog_Input& first, Beluga_Analog_Input& second) // nothrow
{
    // enable ADL (not necessary in our case, but good practice)
    using std::swap;

    swap(static_cast<Beluga_Sensor&>(first), static_cast<Beluga_Sensor&>(second));

    swap(first._pin, second._pin);   
}

//copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
Beluga_Analog_Input& Beluga_Analog_Input::operator=(Beluga_Analog_Input other) 
{
    swap(*this, other); 
    return *this;
}

bool Beluga_Analog_Input::initialise(std::string config_file_path, std::string config_file_section)
{
    //Serial.println("Beluga analog input init");
    _config_file_path = config_file_path;
    _config_file_section = config_file_section;
    Beluga_Ini_Reader ini(config_file_path);
    bool ini_ok = ini.initialise(); //Will always be true, else the ini.initialise() will be in an endless loop of failure.

    bool config_ok = false;
    std::string config_val;
    config_ok = ini.get_config_value(config_file_section, "serial_debug_enable", &config_val );
    if(config_ok)
    {
        set_serial_debug_enable(config_val);
    }    

    _pin.set_pin_direction(INPUT);
    _pin.initialise(config_file_path, config_file_section);
    _pin.configure();

    Beluga_State s;
    _states[_config_file_section] = s;

    return true;
}


bool Beluga_Analog_Input::get_state(int16_t & return_val)
{
    return get_int16_state(_config_file_section, return_val);
}


bool Beluga_Analog_Input::run(void * p )
{
    if (!_pin.get_is_configured())
    {
        return false;
    }
    int16_t analog_val;
    bool read_ok = _pin.analog_read(analog_val);
    if(read_ok)
    {
        bool stored_ok = set_state(analog_val);// [_config_file_section].set_int16_state(analog_val);
        if (! stored_ok)
        {
            Serial.println("Could not store analog input state!");
        }
    }
    return true;
}

bool Beluga_Analog_Input::set_state(int16_t s)
{
    return _states[_config_file_section].set_int16_state(s);
}

#endif