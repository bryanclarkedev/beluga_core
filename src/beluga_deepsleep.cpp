#include "beluga_deepsleep.h"
#include "Arduino.h"
#include "beluga_debug.h"
//====Deepsleep variables====
RTC_DATA_ATTR uint16_t boot_count;

namespace beluga_core
{

    //copy-and-sep for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    void swap(deepsleep& first, deepsleep& second) // nothrow
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        swap(static_cast<mechanism<uint16_t> &>(first), static_cast<mechanism<uint16_t> &>(second));
        swap(first._wake_dt_ms, second._wake_dt_ms);
        swap(first._wake_iterations, second._wake_iterations);
        swap(first._mode, second._mode);
        swap(first._enable_wake_button, second._enable_wake_button);
        swap(first._wake_button_pin_number, second._wake_button_pin_number);
        swap(first._deepsleep_immediately_when_triggered, second._deepsleep_immediately_when_triggered);
        swap(first._wake_duration_threshold, second._wake_duration_threshold);
        swap(first._sleep_duration_s, second._sleep_duration_s);
        swap(first._prev_time_ms, second._prev_time_ms);

        
    }


    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    deepsleep& deepsleep::operator=(deepsleep other) 
    {
        swap(*this, other); 
        return *this;
    }

    bool deepsleep::read_config_sleep_duration()
    {

        std::string sleep_duration_s_str;
        bool sleep_duration_s_ok =  _ini_ptr->get_config_value(_config_file_section, "sleep_duration_s", &sleep_duration_s_str);
        if(sleep_duration_s_ok)
        {
            _sleep_duration_s = (uint16_t) beluga_utils::string_to_int(sleep_duration_s_str);
        }

        return true;
    }

    bool deepsleep::read_config_wake_duration()
    {

        std::string duration_mode_str;
        bool duration_mode_ok = _ini_ptr->get_config_value(_config_file_section, "wake_duration_mode", &duration_mode_str);
        std::string wake_duration_val_str;
        bool wake_duration_val_ok = _ini_ptr->get_config_value(_config_file_section, "wake_duration", &wake_duration_val_str);

        if(duration_mode_ok && wake_duration_val_ok)
        {
            if(duration_mode_str == "n_iterations")
            {
                _mode = deepsleep_wake_duration_mode::n_iterations;
                _wake_duration_threshold = beluga_utils::string_to_int(wake_duration_val_str);
            }else if(duration_mode_str == "duration_s")
            {
                _mode = deepsleep_wake_duration_mode::duration_s;
                _wake_duration_threshold = beluga_utils::string_to_int(wake_duration_val_str);
            }else{
                beluga_utils::debug_print_loop_forever("Something went wrong setting wake_duration_mode: unrecognised value.");
            }
        }
        return true;
    }

    bool deepsleep::read_config_wake_button()
    {
        //Wakeup methods
        std::string enable_wake_button_val_str;
        bool enable_wake_button_ok = _ini_ptr->get_config_value(_config_file_section, "enable_wake_button", &enable_wake_button_val_str );
        if(enable_wake_button_ok)
        {
            _enable_wake_button = beluga_utils::string_to_bool(enable_wake_button_val_str);
            if(_enable_wake_button)
            {
                std::string wake_button_pin_number_str;
                bool wake_button_pin_ok = _ini_ptr->get_config_value(_config_file_section, "wake_button_pin_number", &wake_button_pin_number_str );
                if(wake_button_pin_ok)
                {
                    _wake_button_pin_number = beluga_utils::string_to_int(wake_button_pin_number_str);
                    pinMode(_wake_button_pin_number, INPUT);
                }else{
                    beluga_utils::debug_print_loop_forever("Error reading wake_button_pin_number");
                }
            }
        }
        return true;
    }
    
    bool deepsleep::read_config()
    {
        beluga_core::device::read_config();

        read_config_sleep_duration();
        read_config_wake_duration();
        read_config_wake_button();

        std::string deepsleep_immediately_when_triggered_str;
        bool deepsleep_immediately_when_triggered_ok = _ini_ptr->get_config_value(_config_file_section, "deepsleep_immediately_when_triggered", &deepsleep_immediately_when_triggered_str );
        if(deepsleep_immediately_when_triggered_ok)
        {
            bool do_sleep_immediately = beluga_utils::string_to_bool(deepsleep_immediately_when_triggered_str);
            _deepsleep_immediately_when_triggered = do_sleep_immediately;
        }

        std::string reason;
        get_wakeup_reason(reason);
        Serial.println(reason.c_str());

        configure_deepsleep();
        _prev_time_ms = millis();
        return true;
    }




    bool deepsleep::get_wakeup_reason(std::string & return_val){
        esp_sleep_wakeup_cause_t wakeup_reason;

        wakeup_reason = esp_sleep_get_wakeup_cause();
        
        switch(wakeup_reason)
        {
            case ESP_SLEEP_WAKEUP_EXT0 : return_val = std::string("Wakeup caused by external signal using RTC_IO"); break;
            case ESP_SLEEP_WAKEUP_EXT1 : return_val = std::string("Wakeup caused by external signal using RTC_CNTL"); break;
            case ESP_SLEEP_WAKEUP_TIMER : return_val = std::string("Wakeup caused by timer"); break;
            case ESP_SLEEP_WAKEUP_TOUCHPAD : return_val = std::string("Wakeup caused by touchpad"); break;
            case ESP_SLEEP_WAKEUP_ULP : return_val = std::string("Wakeup caused by ULP program"); break;
            default : 
                _ss.str("");
                _ss << "Wakeup was not caused by deep sleep: " << wakeup_reason;
                return_val = _ss.str();
                _ss.str("");
                break;
        }
        return true;
    }


    void deepsleep::configure_deepsleep()
    {
         /*
        First we configure the wake up source
        We set our ESP32 to wake up for an external trigger.
        There are two types for ESP32, ext0 and ext1 .
        ext0 uses RTC_IO to wakeup thus requires RTC peripherals
        to be on while ext1 uses RTC Controller so doesnt need
        peripherals to be powered on.
        Note that using internal pullups/pulldowns also requires
        RTC peripherals to be turned on.
        */
        //button 1 is unpressed, button 0 is pressed
        if(_enable_wake_button == true)
        {
            esp_sleep_enable_ext0_wakeup((gpio_num_t) _wake_button_pin_number,0); //1 = High (unpressed), 0 = Low (pressed)
        }
        /*
        we configure the wake up source
        We set our ESP32 to wake up every _SLEEP_DURATION_S seconds
        */
        esp_sleep_enable_timer_wakeup(_sleep_duration_s * us_per_s);

        _ss.str("");
        _ss << "Setup ESP32 to sleep for " << _sleep_duration_s << " seconds";
        Serial.println(_ss.str().c_str());
    }


    //Return TRUE if should go to sleep now; return false if not.
    bool deepsleep::run(void *)
    {
        if(! _enabled)
        {
            Serial.println("Seepsleep not enabled");
            return false;
        }
        if(_mode == deepsleep_wake_duration_mode::n_iterations)
        {
            return run_deepsleep_iterations();
        }
        if(_mode == deepsleep_wake_duration_mode::duration_s )
        {
            return run_deepsleep_time();
        }
        assert(false); //Something is very wrong to get here
    }

    void deepsleep::commence_deepsleep()
    {
        //Wifi.disconnect(true);
        //Wifi.mode(WIFI_OFF);
        ////btStop();
        //esp_bluedroid_disable();
        //esp_bt_controller_disable();
        //esp_wifi_stop();
        esp_deep_sleep_start();
    }

    bool deepsleep::run_deepsleep_time()
    {
        unsigned long time_now_ms = millis();
        unsigned long this_dt_ms = time_now_ms - _prev_time_ms;
        _prev_time_ms = time_now_ms;
        _wake_dt_ms += this_dt_ms;
        unsigned long time_awake_s = _wake_dt_ms / 1000;
        if(time_awake_s >= _wake_duration_threshold)
        {
            if(_deepsleep_immediately_when_triggered)
            {
                commence_deepsleep();
            }
            return true; //Return true indicating that we SHOULD DEEPSLEEP!!!
        }
        return false;
    }

    bool deepsleep::run_deepsleep_iterations()
    {
        _wake_iterations++;
        if(_wake_iterations >= _wake_duration_threshold)
        {
            if(_deepsleep_immediately_when_triggered)
            {
                commence_deepsleep();
            }
            return true;
        }
        return false;
    }

}