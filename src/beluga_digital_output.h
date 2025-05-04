#pragma once

#pragma once
#include <string> //For stoi
#include "beluga_mechanism.h"
#include "beluga_gpio.h"


namespace beluga_core
{

    class digital_output : public mechanism<bool>
    {
        public:
            digital_output(){};
            //Copy constructor
            digital_output(const digital_output &b) = default;
            //operator=
            digital_output& operator=(digital_output other);
            //Swap for operator=
            friend void swap(digital_output& first, digital_output& second); 

            bool run(void * p = nullptr );
            virtual bool read_config();
        protected:
            beluga_core::gpio _pin;
        
    };

}