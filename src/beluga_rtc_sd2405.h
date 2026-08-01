#pragma once

#include <string> //For stoi
#include "beluga_mechanism.h"
#include "SD2405.h" //From https://github.com/RobTillaart/SD2405

//We use a global to check whether Wire is running; if it's not we call Wire.begin() based on our own config

namespace beluga_core
{
    class rtc_sd2405 : public beluga_core::mechanism<int>
    {
        public:
            rtc_sd2405(){};
            //Copy constructor
            rtc_sd2405(const rtc_sd2405 &b) = default;
            //operator=
            rtc_sd2405& operator=(rtc_sd2405 other);
            //Swap for operator=
            friend void swap(rtc_sd2405& first, rtc_sd2405& second); 
            virtual bool run(void * p = nullptr );
            virtual bool read_config();
            bool get_time_string(std::string &s);
            bool get_date_string(std::string & s);
            bool get_date_time_string(std::string & s);            
            void generate_report_string();
            
        protected:
            std::string get_padded_time_digit(int i);

            //Change parent class mechanism's set_value public->protected because this is a sensor and we don't want
            //external users trying to set its value
            //Copied from https://stackoverflow.com/questions/2986891/how-to-publicly-inherit-from-a-base-class-but-make-some-of-public-methods-from-t
            using beluga_core::mechanism<int>::set_value;

            std::map<int, std::string> _day_count_to_string;
            SD2405 _rtc;
            bool _set_time = false;
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