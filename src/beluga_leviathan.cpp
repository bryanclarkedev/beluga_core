#include "beluga_leviathan.h"

/*
connect to mqtt client with UI
subscribe to: beluga/leviathan1/#
Publish on: beluga/leviathan1
Message: setpoint=1
*/
namespace beluga_core
{
    bool leviathan::read_config()
    {
        bool machinery_ok = beluga_core::machinery::read_config();
        bool led_ok = false;
        bool fuel_gauge_ok = false;
        bool rtc_ok = false;
        bool deepsleep_ok = false;
        bool temperature_pressure_ok = false;
        auto iter1 = _subdevice_names.begin();
        auto iter2 = _subdevice_types.begin();       
        for( ; iter1 != _subdevice_names.end() && iter2 != _subdevice_types.end(); ++iter1, ++iter2)
        {
            const std::string this_subdevice_name = *iter1;
            const std::string this_subdevice_type = *iter2;
            switch(beluga_utils::djb_hash_for_switch(this_subdevice_type.c_str()))
            {
                case beluga_utils::djb_hash_for_switch("digital_output"):
                {
                    led_ok = true;
                    _led_name = this_subdevice_name;
                    _init_ok[_led_name] = true;
                    break;
                }
                case beluga_utils::djb_hash_for_switch("lipo_fuel_gauge"):
                {
                    fuel_gauge_ok = true;
                    _fuel_gauge_name = this_subdevice_name;
                    break;
                }
                case beluga_utils::djb_hash_for_switch("sd2405_rtc"):
                {
                    rtc_ok = true;
                    _rtc_name = this_subdevice_name;
                    break;
                }
                case beluga_utils::djb_hash_for_switch("deepsleep"):
                {
                    deepsleep_ok = true;
                    _deepsleep_name = this_subdevice_name;
                    break;
                }
                case beluga_utils::djb_hash_for_switch("temperature_pressure"):
                {
                    temperature_pressure_ok = true;
                    _temperature_pressure_name = this_subdevice_name;
                    break;
                }                
                default:
                    break;
            }
        }

        std::map<std::string, bool> = {{_led_name, led_ok}, {_fuel_gauge_name}}
        std::vector<bool> init_ok1{led_ok, fuel_gauge_ok, rtc_ok, deepsleep_ok, temperature_pressure_ok};
        _ss.str("Failed initialisation: ");
        bool any_failed_initialisation = false;
        for(auto i = init_ok1.begin(); i != init_ok1.end(); i++)
        {
            if (*i != true)
            {
                if(any_failed_initialisation)
                {
                    _ss << ", " << 
                }
                any_failed_initialisation = true;
                
                _ss << 

            }
        }
        if(any_failed_initialisation)
        {
                error_loop_forever("leviathan error: could not identify all subdevices.");

        }
        std::shared_ptr<beluga_core::device> this_device;        
        led_ok = get_subdevice(_led_name, _led_ptr);
        fuel_gauge_ok = get_subdevice(_fuel_gauge_name, _fuel_gauge_ptr);
        rtc_ok = get_subdevice(_rtc_name, _rtc_ptr);
        deepsleep_ok = get_subdevice(_deepsleep_name, _deepsleep_ptr);
        temperature_pressure_ok = get_subdevice(_temperature_pressure_name, _temperature_pressure_ptr);
        std::vector<bool> init_ok2{led_ok, fuel_gauge_ok, rtc_ok, deepsleep_ok, temperature_pressure_ok};
        for(auto i = init_ok2.begin(); i != init_ok2.end(); i++)
        {
            if (*i != true)
            {
                error_loop_forever("leviathan error: could not get all pointers to subdevices.");
            }
        }

        return machinery_ok;

    }

    #if 0
    bool leviathan::handle_primary_rx(std::vector< std::map<std::string, std::string> > & rx_subtopic_map_vec)
    {
        for(auto rx_map_iter = rx_subtopic_map_vec.begin(); rx_map_iter != rx_subtopic_map_vec.end(); rx_map_iter++) //Iterate through vector
        {
            
            for (auto m_iter = rx_map_iter->begin(); m_iter != rx_map_iter->end(); m_iter++) //Iterate through map
            {
                if((m_iter->first) == "setpoint")
                {
                    Serial.println("---------------------------------------------------LED set: ");
                    Serial.print("LED set: ");
                    int led_setpoint  = (int) beluga_utils::string_to_int(m_iter->second);
                    Serial.println(led_setpoint);
                    bool setpoint_ok = _led_ptr->set_value((bool) led_setpoint);
                    
                    if(! setpoint_ok)
                    {
                        Serial.println("Something wrong in led setpoint!");
                    }
                }
                #if 0
                if((m_iter->first) == "sleep_now")
                {
                    uint16_t wake_iter_threshold_before_sleep;
                    bool got_thresh_ok = _deepsleep_ptr->get_value(wake_iter_threshold_before_sleep, "wake_iterations_threshold");
                    uint16_t sleep_now_iter_val = wake_iter_threshold_before_sleep;
                    _deepsleep_ptr->set_value(sleep_now_iter_val, "wake_iterations_count");
                    _deepsleep_ptr->run();
                }
                #endif
            }
        }
        return true;
    }
    #endif
    bool leviathan::handle_primary_rx()
    {
        for(auto iter = _primary_rx_list.begin(); iter != _primary_rx_list.end(); iter++)
        {
            std::vector<std::string> split_data =  beluga_utils::spit_from_primary_r(*iter);
            bool processed_ok = split_data.size() == 3;
            if(! processed_ok)
            {
                continue;
            }
            std::string device_name = split_data[0];
            std::string topic_name = split_data[1];
            std::string payload = split_data[2];
            if(device_name == _led_name)
            {
                if(topic_name == "setpoint")
                {
                    Serial.println("---------------------------------------------------LED set: ");
                    Serial.print("LED set: ");
                    int led_setpoint  = (int) beluga_utils::string_to_int(payload);
                    Serial.println(led_setpoint);
                    bool setpoint_ok = _led_ptr->set_value((bool) led_setpoint);
                    
                    if(! setpoint_ok)
                    {
                        Serial.println("Something wrong in led setpoint!");
                    }
                }
            }
        }
    }


    bool leviathan::generate_primary_tx()
    {
        unsigned long time_now_ms = millis();
        //Only report every 1s
        bool run_now = false;
        unsigned long dt_ms = time_now_ms - _prev_loop_time_ms;

        if((dt_ms >= 5000) || (_prev_loop_time_ms == 0) || (time_now_ms % 5000 == 0))
        {
            run_now = true;
            _prev_loop_time_ms = time_now_ms;
        }
        if(! run_now)
        {
            /*
            _ss.str("");
            _ss << "Prev time: " << _prev_loop_time_ms << " Now time: " << time_now_ms << " dt_ms: " << dt_ms;
            Serial.println(_ss.str().c_str());
            */
            return false;
        }
        
        //Add to the tx interthread buffer
        std::map<std::string, std::string> tx_map;
        //-----------Timestamp------------------
        _ss.str("");
        _ss << time_now_ms;
        tx_map["time_ms"] = _ss.str(); // time_now_ms;
        _ss.str("");
        ///------------Iteration-------------------
         _ss.str("");
        _ss << _iteration;
        tx_map["iteration"] = _ss.str();
        _ss.str("");

        //------------------------LED---------------
        bool led_state;
        bool got_state_ok = _led_ptr->get_value(led_state);
        if(got_state_ok)
        {
            _ss.str("");
            _ss << (int) led_state;
            tx_map["led_state"] = _ss.str();
            _ss.str("");
        }
        
        //--------------Time--------------------
        //Read rtc once every 60s
        bool read_rtc = false;
        if(((time_now_ms - _prev_rtc_time_ms) > 60000) || (_prev_rtc_time_ms == 0))
        {
            read_rtc = true;
            _prev_rtc_time_ms = time_now_ms;
        }
        if(read_rtc)
        {
            float time_meas_ms = millis();
            char tempString[15];
            dtostrf(time_meas_ms, 1, 2, tempString);
            Serial.print("time_meas_ms: ");
            Serial.println(tempString);
            //this_mqtt.send_to_mqtt("sensor_station/sense/uptime_ms", tempString);
            std::string datetime_str;
            bool datetime_ok = _rtc_ptr->get_date_time_string(datetime_str);
            if(datetime_ok)
            {
                Serial.print("RTC: ");
                Serial.println(datetime_str.c_str());
                tx_map["rtc_datetime"] = datetime_str;
                //this_mqtt.send_to_mqtt("sensor_station/sense/datetime", datetime_str);
                //_ss << _subtopic_delimiter <<  "rtc_datetime" << _key_val_delimiter << datetime_str;
            }
        }

        //-----------------------Fuel gauge------------------
        bool got_fuel = _fuel_gauge_ptr->run();
        if(got_fuel)
        {
            float V;
            bool got_V_ok = _fuel_gauge_ptr->get_value(V, "voltage_V");
            float percent;
            bool got_percent_ok = _fuel_gauge_ptr->get_value(percent, "percentage");
            if(got_V_ok && got_percent_ok)
            {
                _ss.str("");
                _ss << percent;
                tx_map["battery_percentage"] = _ss.str();
                _ss.str("");
                _ss << V;
                tx_map["battery_V"] = _ss.str();

                //_ss << _subtopic_delimiter << "battery_V" << _key_val_delimiter << V;
                //_ss << _subtopic_delimiter << "battery_percent" << _key_val_delimiter << percent;
            }
        }

        //---------------------Temperature pressure------------------------
        bool got_temperature_pressure = _temperature_pressure_ptr->run();
        if(got_temperature_pressure)
        {
            float temperature_C;
            bool got_temperature_C_ok = _temperature_pressure_ptr->get_value(temperature_C, "temperature_C");
            float pressure_Pa;
            bool got_pressure_ok = _temperature_pressure_ptr->get_value(pressure_Pa, "pressure_Pa");
            _ss.str("");
            if(got_temperature_C_ok && got_pressure_ok)
            {
                _ss.str("");
                _ss << temperature_C;
                tx_map["temperature_C"] = _ss.str();
                _ss.str("");
                _ss << pressure_Pa;
                tx_map["pressure_Pa"] = _ss.str();
            }
        }
        #if 0
        Serial.println("Running main loop deepsleep...");
        bool going_to_sleep = _deepsleep_ptr->run();
        if(! going_to_sleep)
        {
            uint16_t boot_tally;
            bool got_tally_ok = _deepsleep_ptr->get_value(boot_tally, "boot_count");
            uint16_t duration_mode;
            bool got_mode_ok = _deepsleep_ptr->get_value(duration_mode, "duration_mode");
            bool got_duration_ok = false;
            uint16_t iter_tally;
            uint16_t time_elapsed_s;
            if(duration_mode == deepsleep_duration_mode::iteration_duration)
            {
                got_duration_ok = _deepsleep_ptr->get_value(iter_tally, "wake_iterations_count");            
            }
            if(duration_mode == deepsleep_duration_mode::time_duration)
            {
                got_duration_ok = _deepsleep_ptr->get_value(time_elapsed_s, "wake_duration_elapsed_time_s");            
            }
            if(got_tally_ok && got_duration_ok)
            {
                _ss.str("");
                _ss << boot_tally;
                tx_map["boot_tally"] = _ss.str();
                if(duration_mode == deepsleep_duration_mode::iteration_duration)
                {
                    _ss.str("");
                    _ss << iter_tally;
                    tx_map["iterations"] = _ss.str();
                    _ss.str("");
                }
                if(duration_mode == deepsleep_duration_mode::time_duration)
                {
                    _ss.str("");
                    _ss << time_elapsed_s;
                    tx_map["wake_time_elapsed_s"] = _ss.str();
                    _ss.str("");
                }
            }
        }
        #endif


        std::string tx_str;
        bool serialised_ok = _parser.serialise(tx_map, _tx_mail_topic, tx_str);
        if(serialised_ok)
        {
            _primary_tx_list.push_back(tx_str);
        }
        #if 0
            //Topic and subtopic delim
            _ss.str("");
            std::string _topic_delimiter = "@";
            std::string _key_val_delimiter = "=";
            std::string _subtopic_delimiter = "|";
            _ss << _tx_mail_topic << _topic_delimiter << "time_ms" << _key_val_delimiter << time_now_ms;
            //--------------Time--------------------
            float time_meas_ms = millis();
            char tempString[15];
            dtostrf(time_meas_ms, 1, 2, tempString);
            Serial.print("time_meas_ms: ");
            Serial.println(tempString);
            //this_mqtt.send_to_mqtt("sensor_station/sense/uptime_ms", tempString);
            std::string datetime_str;
            bool datetime_ok = _rtc_ptr->get_date_time_string(datetime_str);
            if(datetime_ok)
            {
                
                Serial.println(datetime_str.c_str());
                //this_mqtt.send_to_mqtt("sensor_station/sense/datetime", datetime_str);
                _ss << _subtopic_delimiter <<  "rtc_datetime" << _key_val_delimiter << datetime_str;
            }

            //-----------------------Fuel gauge------------------
            bool got_fuel = _fuel_gauge_ptr->run();
            if(got_fuel)
            {
                float V;
                bool got_V_ok = _fuel_gauge_ptr->get_value(V, "voltage_V");
                float percent;
                bool got_percent_ok = _fuel_gauge_ptr->get_value(percent, "percentage");
                if(got_V_ok && got_percent_ok)
                {
                    _ss << _subtopic_delimiter << "battery_V" << _key_val_delimiter << V;
                    _ss << _subtopic_delimiter << "battery_percent" << _key_val_delimiter << percent;
                }
            }

            //-------------------Add to primary tx---------------
            _primary_tx_list.push_back(_ss.str());
            _ss.str("");
        #endif



        return true;
    }
    
  
    bool leviathan::run(void * p )
    {
   

        //Handle rx
        std::vector< std::map<std::string, std::string> >  rx_subtopic_map_vec;
        bool got_rx_for_us = parse_primary_mqtt_rx(rx_subtopic_map_vec);



        if(got_rx_for_us)
        {
            handle_primary_rx(rx_subtopic_map_vec);
        }


     
        //Run subdevices
        bool run_ok = beluga_core::machinery::run();

        /*
        //Run comms rx
        for(auto comms_iter = _comms_map.begin(); comms_iter != _comms_map.end(); comms_iter++)
        {
            comms_iter->second->run_rx();
        }
        //Run devices
        for(auto subdevice_iter = _subdevices.begin(); subdevice_iter != _subdevices.end(); subdevice_iter++)
        {
            if(subdevice_iter->first == _deepsleep_name)
            {
                continue;
            }
            subdevice_iter->second->run(p);
        }
        //Comms are devices and so have a run() also
        for(auto comms_iter = _comms_map.begin(); comms_iter != _comms_map.end(); comms_iter++)
        {
            comms_iter->second->run();
        }
        //Run comms tx
        for(auto comms_iter = _comms_map.begin(); comms_iter != _comms_map.end(); comms_iter++)
        {
            comms_iter->second->run_tx();
        }
        */


        //Populate tx
        generate_primary_tx();

        _iteration++;


        //Generate JSON
        return true;
    }


}