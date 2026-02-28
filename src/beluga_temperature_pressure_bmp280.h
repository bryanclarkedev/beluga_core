#pragma once

#include <string> //For stoi
#include "beluga_mechanism.h"
//#include "DFRobot_BMP280.h"
#include "Adafruit_BMP280.h"
#include "beluga_device.h"
#include "Wire.h"

namespace beluga_core
{
    class temperature_pressure_bmp280 : public beluga_core::mechanism<float>
    {
        public:
            temperature_pressure_bmp280() //: _pressure_temperature(&Wire, DFRobot_BMP280_IIC::eSdoLow)
                {
                };
            //Copy constructor
            temperature_pressure_bmp280(const temperature_pressure_bmp280 &b) = default;
            //operator=
            temperature_pressure_bmp280& operator=(temperature_pressure_bmp280 other);
            //Swap for operator=
            friend void swap(temperature_pressure_bmp280& first, temperature_pressure_bmp280& second); 
            virtual bool run(void * p = nullptr );
            virtual bool read_config();
            //void printLastOperateStatus(DFRobot_BMP280_IIC::eStatus_t eStatus);

        protected:
            //Change parent class mechanism's set_value public->protected because this is a sensor and we don't want
            //external users trying to set its value
            //Copied from https://stackoverflow.com/questions/2986891/how-to-publicly-inherit-from-a-base-class-but-make-some-of-public-methods-from-t
            using beluga_core::mechanism<float>::set_value;

           // DFRobot_BMP280_IIC     _pressure_temperature;
           Adafruit_BMP280 _temperature_pressure;
    };

}


