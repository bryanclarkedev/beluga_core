#include "beluga_gpio.h"
#include "beluga_debug.h"
#include <Arduino.h>
#include <cmath> //For std::pow
#include <algorithm> //For std::min
namespace beluga_core
{
        
    static uint8_t LED_CHANNEL_COUNT = 0;
    const uint8_t MAX_LED_CHANNEL_COUNT = 15;
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
        //swap(first._gpio_map, second._gpio_map);
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
            if(_device_type_str == "analog_output"){
                uint32_t freq_hz = 100;
                if(beluga_core::LED_CHANNEL_COUNT < beluga_core::MAX_LED_CHANNEL_COUNT){
                    _channel = beluga_core::LED_CHANNEL_COUNT;
                    //config_ok = ledcSetup(_channel, _freq_hz, n_bits_resolution, _channel );
                    
                    config_ok = ledcSetup(_channel, freq_hz, _n_bits_resolution); // Setup the channel
                    ledcAttachPin(_pin_number, _channel);       // Attach pin to the channel
                    if(! config_ok){
                        beluga_utils::debug_print_loop_forever("Failure configuring analog output");
                    }
                    beluga_core::LED_CHANNEL_COUNT++;
            }
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
                #ifdef BOARD
                #if BOARD == "ESP32"
                    this_pin = firebeetle_gpio_map[s];
                #else
                    assert(false);
                #endif
                #endif
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
            beluga_utils::debug_print("Could not configure!!!!");
            return false;
        }
        try{
            pinMode(_pin_number, _pin_direction);
            _configured = true;
        }catch(...)
        {
            beluga_utils::debug_print("Error configuring!!!!");
            return false;
        }
        return true;    
    }

    bool gpio::get_is_configured()
    {
        return _configured;
    }

    bool gpio::analog_write(int16_t val){
        if (!_configured)
        {
            return false;
        }
        if(_pin_direction != OUTPUT)
        {
            return false;
        }
        //analogWriteFrequency( 5000);
        //analogWrite(_pin_number, val);
        //ledcAttach(_pin_number, 1000, 8); //_pin number, freq in hz, bits 1-14
        //Max duty cycle == 2**n_bits_resolution - 1
        uint16_t max_duty_cycle = (uint16_t) std::pow(2, _n_bits_resolution) - 1;
        if(val > max_duty_cycle){
            val = max_duty_cycle;
        }
        //uint16_t duty_cycle = std::min(val, max_duty_cycle);
        ledcWrite(_channel, val);
        
        return true;

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