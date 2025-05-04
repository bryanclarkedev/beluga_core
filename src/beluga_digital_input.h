#pragma once

#pragma once
#include <string> //For stoi
#include "beluga_mechanism.h"
#include "beluga_gpio.h"


namespace beluga_core
{

    class digital_input : public mechanism<bool>
    {
        public:
            digital_input(){};
            //Copy constructor
            digital_input(const digital_input &b) = default;
            //operator=
            digital_input& operator=(digital_input other);
            //Swap for operator=
            friend void swap(digital_input& first, digital_input& second); 

            virtual bool run(void * p = nullptr );
            virtual bool read_config();
            

        protected:
            //Change parent class mechanism's set_value public->protected because this is a sensor and we don't want
            //external users trying to set its value
            //Copied from https://stackoverflow.com/questions/2986891/how-to-publicly-inherit-from-a-base-class-but-make-some-of-public-methods-from-t
            using mechanism<bool>::set_value;

            beluga_core::gpio _pin;
        
    };

}