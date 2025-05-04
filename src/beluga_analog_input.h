#pragma once
#include <string> //For stoi
#include "beluga_mechanism.h"
#include "beluga_gpio.h"


namespace beluga_core
{

    class analog_input : public mechanism<int16_t>
    {
        public:
            analog_input(){};
            //Copy constructor
            analog_input(const analog_input &b) = default;
            //operator=
            analog_input& operator=(analog_input other);
            //Swap for operator=
            friend void swap(analog_input& first, analog_input& second); 

            virtual bool run(void * p = nullptr );
            virtual bool read_config();
            
        protected:
            //Change parent class mechanism's set_value public->protected because this is a sensor and we don't want
            //external users trying to set its value
            //Copied from https://stackoverflow.com/questions/2986891/how-to-publicly-inherit-from-a-base-class-but-make-some-of-public-methods-from-t
            using mechanism::set_value;

            gpio _pin;
            std::string _device_type = "analog_input";
    };

}