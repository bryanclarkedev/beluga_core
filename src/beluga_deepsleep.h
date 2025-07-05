#pragma once
#include "Arduino.h"


#include <string> //For stoi
#include "beluga_mechanism.h"

/*
Simple Deep Sleep with Timer Wake Up
=====================================
ESP32 offers a deep sleep mode for effective power
saving as power is an important factor for IoT
applications. In this mode CPUs, most of the RAM,
and all the digital peripherals which are clocked
from APB_CLK are powered off. The only parts of
the chip which can still be powered on are:
RTC controller, RTC peripherals ,and RTC memories

This code displays the most basic deep sleep with
a timer to wake it up and how to store data in
RTC memory to use it over reboots

This code is under Public Domain License.

Author:
Pranav Cherukupalli <cherukupallip@gmail.com>
*/
//Deep sleep stuff - time-based wake
#define s_to_us_factor 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define ms_to_us_factor 1000

enum deepsleep_duration_mode { time_duration, iteration_duration };





namespace beluga_core
{
    class deepsleep : public mechanism<uint16_t>
    {
        
        public:
            deepsleep(){};
            //Copy constructor
            deepsleep(const deepsleep &b) = default;
            //operator=
            deepsleep& operator=(deepsleep other);
            //Swap for operator=
            friend void swap(deepsleep& first, deepsleep& second); 
            bool run(void * p = nullptr );
            virtual bool read_config();
            bool get_wakeup_reason(std::string & return_val);

            void commence_deepsleep();
            void print_wakeup_reason();

        protected:
            void configure_deepsleep();
            bool run_deepsleep_iterations();
            bool run_deepsleep_time();
            unsigned long _dt_ms = 0;
            deepsleep_duration_mode _mode = deepsleep_duration_mode::time_duration;

            bool _enable_wake_button = false;
            uint16_t _wake_button_pin_number = 0;
            const uint16_t WAKE_ITERATIONS_THRESHOLD_DEFAULT = 50;
            const uint16_t WAKE_TIME_THRESHOLD_S_DEFAULT = 60;
            //6 minutes = 360s = 360000ms
            const uint16_t SLEEP_DURATION_S_DEFAULT = 300; //000 * ms_to_us_factor;
            const uint16_t SLEEP_IMMEDIATELY = 1;
            const uint16_t DO_NOT_SLEEP_IMMEDIATELY = 0;
            std::map<bool, uint16_t> deepsleep_immediately_map;
    };
}