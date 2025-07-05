#include "beluga_deepsleep.h"
#include "Arduino.h"

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

    }

    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    deepsleep& deepsleep::operator=(deepsleep other) 
    {
        swap(*this, other); 
        return *this;
    }

    
    bool deepsleep::read_config()
    {
        beluga_core::device::read_config();

        add_new_value("duration_mode");

        add_new_value("wake_duration_elapsed_time_s");
        add_new_value("wake_duration_time_threshold_s");
        
        add_new_value("wake_iterations_count");
        add_new_value("wake_iterations_threshold");
        add_new_value("sleep_duration_s");
        add_new_value("boot_count");
        add_new_value("go_to_sleep_immediately");

        set_value(0, "wake_iterations_count");
        set_value(0, "wake_duration_elapsed_time_s");
        
        set_value(WAKE_ITERATIONS_THRESHOLD_DEFAULT, "wake_iterations_threshold");
        set_value(WAKE_TIME_THRESHOLD_S_DEFAULT, "wake_duration_time_threshold_s");
        set_value(SLEEP_DURATION_S_DEFAULT, "sleep_duration_s");
        //Increment, and Copy from persistent memory -> volatile memory
        boot_count++;
        set_value(boot_count, "boot_count");
        deepsleep_immediately_map[true] = SLEEP_IMMEDIATELY;
        deepsleep_immediately_map[false] = DO_NOT_SLEEP_IMMEDIATELY;
        set_value(deepsleep_immediately_map[true], "go_to_sleep_immediately"); //By default, sleep immediately when condition met. If False, reliant on external call to trigger sleep.

        //bool ini_ok = _ini_ptr->initialise(); //Will always be true, else the ini.initialise() will be in an endless loop of failure.

  
        std::string enable_wake_button_val_str;
        bool enable_wake_button_ok = _ini_ptr->get_config_value(_config_file_section, "enable_wake_button", &enable_wake_button_val_str );
        if(enable_wake_button_ok)
        {
            bool _enable_wake_button = beluga_utils::string_to_bool(enable_wake_button_val_str);
            if(_enable_wake_button)
            {
                std::string wake_button_pin_number_str;
                bool wake_button_pin_ok = _ini_ptr->get_config_value(_config_file_section, "wake_button_pin_number", &wake_button_pin_number_str );
                if(wake_button_pin_ok)
                {
                    _wake_button_pin_number = beluga_utils::string_to_int(wake_button_pin_number_str);
                    pinMode(_wake_button_pin_number, INPUT);

                }
            }
        }

        //Two possible ways to define wake duration: iterations and time. Time is default
        bool got_wake_iterations_threshold = false;
        bool got_wake_time_threshold = false;
        bool wake_iterations_threshold_ok = false;
        std::string wake_time_threshold_str;
        bool wake_time_threshold_ok = _ini_ptr->get_config_value(_config_file_section, "wake_duration_time_threshold_s", &wake_time_threshold_str);
        if(wake_time_threshold_ok)
        {
            set_value(deepsleep_duration_mode::time_duration, "duration_mode");
            _mode = deepsleep_duration_mode::time_duration;
            set_value(beluga_utils::string_to_int(wake_time_threshold_str), "wake_duration_time_threshold_s");
        }else{
            std::string wake_iterations_threshold_str;
            wake_iterations_threshold_ok =  _ini_ptr->get_config_value(_config_file_section, "wake_iterations_threshold", &wake_iterations_threshold_str );
            if(wake_iterations_threshold_ok)
            {
                
                set_value(deepsleep_duration_mode::iteration_duration, "duration_mode");
                _mode = deepsleep_duration_mode::iteration_duration;
                set_value(beluga_utils::string_to_int(wake_iterations_threshold_str), "wake_iterations_threshold");
            }
        }
        if((! wake_time_threshold_ok) && (! wake_iterations_threshold_ok))
        {
            debug_print_loop_forever("deepsleep: no wake duration set in either seconds or iterations. Config must have either wake_duration_time_threshold_s or wake_iterations_threshold");
        }
    
        std::string sleep_duration_s_str; //Note: seconds
        bool sleep_duration_s_ok =  _ini_ptr->get_config_value(_config_file_section, "sleep_duration_s", &sleep_duration_s_str );
        if(sleep_duration_s_ok)
        {
            set_value(beluga_utils::string_to_int(sleep_duration_s_str), "sleep_duration_s");
        }else{
            debug_print_loop_forever("deepsleep: no sleep_duration_s set in the config file.");
        }

        std::string sleep_immediately_on_timeout_str;
        bool sleep_immediately_on_timeout_ok = _ini_ptr->get_config_value(_config_file_section, "sleep_immediately_on_timeout", &sleep_duration_s_str );
        if(sleep_immediately_on_timeout_ok)
        {
            bool do_sleep_immediately = beluga_utils::string_to_bool(sleep_duration_s_str);
            set_value(deepsleep_immediately_map[do_sleep_immediately], "go_to_sleep_immediately");
        }

        print_wakeup_reason();

        configure_deepsleep();

        return true;
    }


    /*
    Method to print the reason by which ESP32
    has been awaken from sleep

    Copied verbatime from example
    */
    void deepsleep::print_wakeup_reason(){
        esp_sleep_wakeup_cause_t wakeup_reason;

        wakeup_reason = esp_sleep_get_wakeup_cause();
        
        switch(wakeup_reason)
        {
            case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
            case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
            case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
            case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
            case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
            default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
        }
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
       uint16_t sleep_duration_s;
       bool got_duration = get_value(sleep_duration_s, "sleep_duration_s");
       if(! got_duration)
       {
        debug_print_loop_forever("Deepsleep error: could not get the sleep duration in ms!");
       }
        esp_sleep_enable_timer_wakeup(sleep_duration_s * s_to_us_factor);

        _ss.str("");
        _ss << "Setup ESP32 to sleep for " << sleep_duration_s << " seconds";
        Serial.println(_ss.str().c_str());
    }


    //Return TRUE if should go to sleep now; return false if not.
    bool deepsleep::run(void *)
    {
        if(! _enabled)
        {
            return false;
        }
        if(_mode == deepsleep_duration_mode::iteration_duration)
        {
            return run_deepsleep_iterations();
        }
        if(_mode == deepsleep_duration_mode::time_duration )
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
        uint16_t wake_time_s;
        get_value(wake_time_s, "wake_duration_elapsed_time_s" );
        beluga_core::value<uint16_t> this_wake_time_elapsed_value;
        //We need to pull out the beluga_core::value<uint16_t> to get its timestamp
        uint16_t this_wake_time_elapsed_s;
        get_value(this_wake_time_elapsed_value, "wake_duration_elapsed_time_s");
        //Extract timestamp and calculate time deltas
        unsigned long prev_time = this_wake_time_elapsed_value.get_value_changed_time_ms();
        unsigned long this_dt_ms = millis() - this_wake_time_elapsed_value.get_value_changed_time_ms();
        _dt_ms += this_dt_ms;
        this_wake_time_elapsed_value.get_value(this_wake_time_elapsed_s);
        this_wake_time_elapsed_s += _dt_ms/1000; //ms to s conversion
        //Save
        set_value(this_wake_time_elapsed_s, "wake_duration_elapsed_s");

        //Check for begin sleep
        uint16_t max_wake_duration_s;
        get_value(max_wake_duration_s, "wake_duration_time_threshold_s");
        if(this_wake_time_elapsed_s >= max_wake_duration_s)
        {
            uint16_t sleep_immediately;
            get_value(sleep_immediately, "go_to_sleep_immediately");
            if(sleep_immediately)
            {
                commence_deepsleep();
            }
            return true; //Return true indicating that we SHOULD DEEPSLEEP!!!
        }
        return false;
    }

    bool deepsleep::run_deepsleep_iterations()
    {
        uint16_t wake_iterations;
        get_value(wake_iterations, "wake_iterations_count" );
        //Serial.print("Wake iterations: " );
        //Serial.println(wake_iterations);
        wake_iterations++;
        set_value(wake_iterations, "wake_iterations_count");
        //Check for begin sleep
        uint16_t max_wake_iterations;
        get_value(max_wake_iterations, "wake_iterations_threshold");

        if(wake_iterations >= max_wake_iterations)
        {
            uint16_t sleep_immediately;
            get_value(sleep_immediately, "go_to_sleep_immediately");
            if(sleep_immediately)
            {
                commence_deepsleep();
            }
            return true;
        }
        return false;
    }

}