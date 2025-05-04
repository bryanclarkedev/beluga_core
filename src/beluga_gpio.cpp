#include "beluga_gpio.h"

namespace beluga_core
{
        
    gpio::gpio()
    {
        #if 0
        //Some #defines for the FireBeetle ESP32 board
        _gpio_map["D0"] = D0;
        _gpio_map["D1"] = D1;
        _gpio_map["D2"] = D2;
        _gpio_map["D3"] = D3;
        #endif
    }

    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    gpio& gpio::operator=(gpio other) 
    {
        swap(*this, other); 

        return *this;
    }

    //copy-and-sep for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    void swap(gpio& first, gpio& second) // nothrow
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;
        
        // by swapping the members of two objects,
        // the two objects are effectively swapped
        swap(static_cast<device &>(first), static_cast<device &>(second));
        swap(first._pin_number, second._pin_number);
        swap(first._pin_direction, second._pin_direction);
        swap(first._gpio_map, second._gpio_map);
        swap(first._pin_number_set, second._pin_number_set);
        swap(first._pin_direction_set, second._pin_direction_set);
        swap(first._configured, second._configured);
    }


    bool gpio::read_config()
    {
        /*
        _config_file_path = config_file_path;
        _config_file_section = config_file_section;
        ini_reader ini(config_file_path);
        */
        bool ini_ok = _ini_ptr->initialise(); //Will always be true, else the ini.initialise() will be in an endless loop of failure.

        bool config_ok = false;
        std::string config_val;
        config_ok = _ini_ptr->get_config_value(_config_file_section, "enable_serial_debug", &config_val );
        if(config_ok)
        {
            set_serial_debug_enabled(config_val);
        }
        config_ok = _ini_ptr->get_config_value(_config_file_section, "pin", &config_val );
        if(config_ok)
        {
            set_pin_number(config_val);
        }
        
        config_ok = _ini_ptr->get_config_value(_config_file_section, "pin_direction", &config_val );
        if(config_ok)
        {
        set_pin_direction(config_val);
        }

        return true;
    }


    bool gpio::set_pin_number(uint8_t p)
    {
        _pin_number = p;
        _pin_number_set = true;
        return true;
    }


    bool gpio::set_pin_number(std::string s)
    {
        s = beluga_utils::string_to_upper(s);
        try{
            uint8_t this_pin;
            if(s[0] == 'D')
            {//Assume Firebeetle format, D1, D2, etc
                this_pin = _gpio_map[s];
            }else{
                //Assume stringified integer.
                this_pin = stoi(s);
            }
            set_pin_number(this_pin);
        }
        catch(...) {//Wildcard catch
            _ss.str("");
            _ss << "Could not set pin from key: " << s;
            throw_line(_ss.str());
            return false;
        }
        return true;
    }

    uint8_t gpio::get_pin_number()
    {
        return _pin_number;
    }

    bool gpio::set_pin_direction(uint8_t dir)
    {
        _pin_direction = dir;
        _pin_direction_set = true;
        return true;
    }

    bool gpio::set_pin_direction(std::string s)
    {
        s = beluga_utils::string_to_upper(s);
        if((s == "IN") || (s == "INPUT"))
        {
            set_pin_direction(INPUT);
        }
        if((s == "OUT") || (s == "OUTPUT"))
        {
            set_pin_direction(OUTPUT);
        }
        if (!_pin_direction_set)
        {
            _ss.str("");
            _ss << "Could not set pin direction from key: " << s;
            _ss << ". Accepted values: INPUT, OUTPUT";
            throw_line(_ss.str());
            return false;
        }

        return true;
    }


    bool gpio::configure()
    {

        if (!( _pin_number_set && _pin_direction_set))
        {
            Serial.println("Could not configure!!!!");
            return false;
        }
        try{
            pinMode(_pin_number, _pin_direction);
            _configured = true;
        }catch(...)
        {
            Serial.println("Error configuring!!!!");
            return false;
        }
        return true;    
    }

    bool gpio::get_is_configured()
    {
        return _configured;
    }



    bool gpio::analog_read(int16_t & return_val)
    {
        if (!_configured)
        {
            return false;
        }
        if(_pin_direction != INPUT)
        {
            return false;
        }
        return_val = analogRead(_pin_number);
        return true;
    }

    bool gpio::digital_read(bool & return_val)
    {
        if (!_configured)
        {
            return false;
        }
        if(_pin_direction != INPUT)
        {
            return false;
        }
        return_val = digitalRead(_pin_number);
        return true;
    }

    bool gpio::digital_write(bool val)
    {
        if (!_configured)
        {
            return false;
        }
        if(_pin_direction != OUTPUT)
        {
            return false;
        }

        digitalWrite(_pin_number, val);
        return true;
    }
}