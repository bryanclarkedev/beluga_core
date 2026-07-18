#pragma once
#if 0
#include <string> //For stoi
#include "beluga_mechanism.h"
#include "Adafruit_BNO055.h.h" //From https://github.com/adafruit/Adafruit_BNO055
//We use a global to check whether Wire is running; if it's not we call Wire.begin() based on our own config

namespace beluga_core
{
    class ahrs_bno055 : public beluga_core::mechanism<double>
    {
        public:
            ahrs_bno055(){};
            //Copy constructor
            ahrs_bno055(const ahrs_bno055 &b) = default;
            //operator=
            ahrs_bno055& operator=(ahrs_bno055 other);
            //Swap for operator=
            friend void swap(ahrs_bno055& first, ahrs_bno055& second); 
            virtual bool run(void * p = nullptr );
            virtual bool read_config();
            
        protected:
            //Change parent class mechanism's set_value public->protected because this is a sensor and we don't want
            //external users trying to set its value
            //Copied from https://stackoverflow.com/questions/2986891/how-to-publicly-inherit-from-a-base-class-but-make-some-of-public-methods-from-t
            using beluga_core::mechanism<double>::set_value;

            Adafruit_BNO055 _ahrs_bno055;
    };

}


#endif