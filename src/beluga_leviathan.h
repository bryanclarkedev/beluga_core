#pragma once
#include "beluga_machinery.h"
#include "beluga_digital_output.h"
#include "beluga_lipo_fuel_gauge.h"
//#include "beluga_sd2405_rtc.h"
#include "beluga_deepsleep.h"
#include "beluga_bmp280_temperature_pressure.h"
#include "beluga_parsers.h"
/*


*/
namespace beluga_core
{
    
    class leviathan : public beluga_core::machinery
    {
        public:
            leviathan(){};
            virtual bool run(void * p = nullptr);
            virtual bool read_config();
            virtual bool handle_primary_rx(); //std::vector< std::map<std::string, std::string> > & rx_subtopic_map_vec);
            virtual bool generate_primary_tx();

        private:
            unsigned long _prev_loop_time_ms = 0;
            unsigned long _prev_rtc_time_ms = 0;
            std::string _led_name = "";
            std::shared_ptr<beluga_core::digital_output> _led_ptr = nullptr;

            std::string _fuel_gauge_name = "";
            std::shared_ptr<beluga_core::lipo_fuel_gauge> _fuel_gauge_ptr;

            std::string _rtc_name = "";
            std::shared_ptr<beluga_core::sd2405_rtc> _rtc_ptr;

            std::string _deepsleep_name = "";
            std::shared_ptr<beluga_core::deepsleep> _deepsleep_ptr = nullptr;

            std::string _temperature_pressure_name = "";
            std::shared_ptr<beluga_extensions::bmp280_temperature_pressure> _temperature_pressure_ptr = nullptr;



    };

}