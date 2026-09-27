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
            using mechanism<int16_t>::set_value;

            beluga_core::gpio _pin;
        
    };

}


#if 0

#pragma once
#include <string> //For stoi
#include "Beluga_Sensor.h"
#include "Beluga_Pin.h"

class Beluga_Analog_Input : public Beluga_Sensor
{
    public:
        Beluga_Analog_Input(){};
        //Copy constructor
        Beluga_Analog_Input(const Beluga_Analog_Input &b) = default;
        //operator=
        Beluga_Analog_Input& operator=(Beluga_Analog_Input other);
        //Swap for operator=
        friend void swap(Beluga_Analog_Input& first, Beluga_Analog_Input& second); 

        virtual bool run(void * p = nullptr );
        virtual bool initialise(std::string config_file_path, std::string config_section);

        virtual bool get_state(int16_t & );
        
        //No float state
        virtual bool get_float_state(std::string s, float & return_val) {return false; }
        
    protected:
        virtual bool set_state(int16_t s);
        Beluga_Pin _pin;

};



#endif