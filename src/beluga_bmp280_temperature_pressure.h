#pragma once

#include <string> //For stoi
#include "beluga_mechanism.h"
#include "DFRobot_BMP280.h"
#include "beluga_device.h"
#include "Wire.h"

namespace beluga_core
{
    class bmp280_temperature_pressure : public beluga_core::mechanism<float>
    {
        public:
            bmp280_temperature_pressure(): _pressure_temperature(&Wire, DFRobot_BMP280_IIC::eSdoLow)
                {
                };
            //Copy constructor
            bmp280_temperature_pressure(const bmp280_temperature_pressure &b) = default;
            //operator=
            bmp280_temperature_pressure& operator=(bmp280_temperature_pressure other);
            //Swap for operator=
            friend void swap(bmp280_temperature_pressure& first, bmp280_temperature_pressure& second); 
            virtual bool run(void * p = nullptr );
            virtual bool read_config();
            void printLastOperateStatus(DFRobot_BMP280_IIC::eStatus_t eStatus);

        protected:
            //Change parent class mechanism's set_value public->protected because this is a sensor and we don't want
            //external users trying to set its value
            //Copied from https://stackoverflow.com/questions/2986891/how-to-publicly-inherit-from-a-base-class-but-make-some-of-public-methods-from-t
            using beluga_core::mechanism<float>::set_value;

            DFRobot_BMP280_IIC     _pressure_temperature;
    };

}


