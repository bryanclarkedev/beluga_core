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
#define us_per_s 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define ms_per_s 1000


namespace beluga_core
{
    class deepsleep : public mechanism<uint16_t>
    {

        enum deepsleep_wake_duration_mode { n_iterations, duration_s };

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
            //void print_wakeup_reason();

        protected:
            void configure_deepsleep();
            bool read_config_sleep_duration();
            bool read_config_wake_duration();
            bool read_config_wake_button();

            bool run_deepsleep_iterations();
            bool run_deepsleep_time();
            unsigned long _wake_dt_ms = 0;
            unsigned long _wake_iterations = 0;
            deepsleep_wake_duration_mode _mode = deepsleep_wake_duration_mode::duration_s;

            bool _enable_wake_button = false;
            uint16_t _wake_button_pin_number = 0;
            bool _deepsleep_immediately_when_triggered = false;
            uint16_t _wake_duration_threshold = 60;
            uint16_t _sleep_duration_s = 60;
            unsigned long _prev_time_ms = 0;
    };
}