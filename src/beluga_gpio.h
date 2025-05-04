#pragma once
#include <stdint.h> //For int16_t
#include <algorithm> //For std::swap
#include <map> //For map
#include "Arduino.h" //For millis()
#include "beluga_device.h"
#include "beluga_string.h"
#include "beluga_exceptions.h"
#include "beluga_ini_reader.h"

/*
This is a gpio interface
It does NOT store values for either setpoints or controls
It just manages setting up the pin (number and direction) and the passthrough of data (reading or writing)
It needs a wrapper around it to handle storing data, and typically that would narrow down what can be done (e.g. it's not likely
that you would need both digital read and analog write on the one pin)

Although it is of type beluga_core::device its run() function should NOT be used. Handle that in a parent object.
*/
namespace beluga_core
{
    class gpio : public device
    {
        public:
            gpio();

            //Copy constructor
            gpio(const gpio &b) = default;
            //operator= is inherited from Beluga_Device as it is just a call to swap()
            //Swap for operator=
            friend void swap(gpio& first, gpio& second); 
            gpio& operator=(gpio other);

            //No run function
            bool run(void * p = nullptr){ return false;}
            bool read_config();
            bool configure();

            bool digital_write(bool val);
            bool digital_read(bool & return_val);
            bool analog_read(int16_t & return_val);
            
            virtual bool set_pin_number(uint8_t s);
            virtual bool set_pin_number(std::string s);
            virtual uint8_t get_pin_number();
            virtual bool set_pin_direction(uint8_t);
            virtual bool set_pin_direction(std::string s);
            
            virtual bool get_is_configured();
        protected:
            uint8_t _pin_number = 0;
            uint8_t _pin_direction;
            bool _pin_number_set = false;
            bool _pin_direction_set = false;
            bool _configured = false;
            std::map<std::string, uint8_t> _gpio_map; //We want to be able to map the firebeetle GPIO shield to pins.

            
    };
}