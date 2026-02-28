#pragma once

#include <string> //For stoi
#include "beluga_mechanism.h"
#include "MAX17043.h"


namespace beluga_core
{
    class li_batt_fuel_gauge : public beluga_core::mechanism<float>
    {
        public:
            li_batt_fuel_gauge(){};
            //Copy constructor
            li_batt_fuel_gauge(const li_batt_fuel_gauge &b) = default;
            //operator=
            li_batt_fuel_gauge& operator=(li_batt_fuel_gauge other);
            //Swap for operator=
            friend void swap(li_batt_fuel_gauge& first, li_batt_fuel_gauge& second); 
            virtual bool run(void * p = nullptr );
            virtual bool read_config();
            
        protected:
            //Change parent class mechanism's set_value public->protected because this is a sensor and we don't want
            //external users trying to set its value
            //Copied from https://stackoverflow.com/questions/2986891/how-to-publicly-inherit-from-a-base-class-but-make-some-of-public-methods-from-t
            using beluga_core::mechanism<float>::set_value;

            MAX17043 _fuel_gauge;
    };

}



#if 0
#include "DFRobot_MAX17043.h"

/*
https://wiki.dfrobot.com/Gravity__3.7V_Li_Battery_Fuel_Gauge_SKU__DFR0563
Input Voltage (VCC): 3.3V~6.0V
Battery Input Voltage (BAT IN): 2.5V~4.2V
Battery Type(BAT IN): 3.7V Li-polymer/Li-ion battery
Operating Current: 50 uA

*/
class Beluga_FuelGauge
{
  public:
    Beluga_FuelGauge( bool enable_debug );
    bool initialise();
    void run();
    float voltage_mV;
    float percentage;
    private:
    bool _enable_debug;
    DFRobot_MAX17043        gauge;



};

#endif