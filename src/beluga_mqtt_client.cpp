#include "beluga_mqtt_client.h"
#include "beluga_debug.h"
#include "beluga_string.h"
#include <WiFi.h>
//#include <MQTT.h>

//https://github.com/knolleary/pubsubclient/blob/master/examples/mqtt_reconnect_nonblocking/mqtt_reconnect_nonblocking.ino


namespace beluga_core
{
    std::list< std::pair<std::string, std::string> > mqtt_client::mqtt_callback_rx_data;

    void mqtt_client::callback(char* topic, byte* message, unsigned int length) {
        Serial.print("Message arrived on topic: ");
        Serial.print(topic);
        Serial.print(". Message: ");
        std::string  message_str;
        std::stringstream parse_ss;
        for (int i = 0; i < length; i++) {
            //Serial.print((char)message[i]);
            parse_ss << (char)message[i];
        }
        message_str = parse_ss.str();
        Serial.println(message_str.c_str());

        std::stringstream ss;
        std::string topic_str(topic);
        std::pair<std::string, std::string> this_pair = std::make_pair(topic_str, message_str);

        mqtt_client::mqtt_callback_rx_data.push_back(this_pair);

    }


    bool mqtt_client::reconnect() {
        if (_mqtt_client.connect(_mqtt_client_name.c_str())) {
            for(int i = 0; i < _rx_topic_list.size(); i++)
            {
                std::string this_topic = _rx_topic_list[i];
                if(_universal_topic_prefix != "")
                {
                    std::stringstream ss;
                    ss << _universal_topic_prefix << this_topic;
                    this_topic = ss.str();
                }
                Serial.print(i);
                Serial.print(": ");
                Serial.println(this_topic.c_str());
                _mqtt_client.subscribe(this_topic.c_str());
            }
        }
        return _mqtt_client.connected();
    }

    //copy-and-sep for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    void swap(mqtt_client & first, mqtt_client& second) // nothrow
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        swap(static_cast<beluga_core::comms&>(first), static_cast<beluga_core::comms&>(second));

        swap(first._tx_time_ms, second._tx_time_ms);
        swap(first._rx_time_ms, second._rx_time_ms);
        swap(first._mqtt_client_name, second._mqtt_client_name);
        swap(first._mqtt_username, second._mqtt_username);
        swap(first._mqtt_password, second._mqtt_password);
        swap(first._mqtt_port_number, second._mqtt_port_number);

        swap(first._mqtt_server_address_str, second._mqtt_server_address_str);
        swap(first._mqtt_server_ip, second._mqtt_server_ip);
        swap(first._wifi_client, second._wifi_client);
        swap(first._mqtt_client, second._mqtt_client);

        swap(first._wifi_network_name, second._wifi_network_name);
        swap(first._wifi_password, second._wifi_password);

    }

        //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    mqtt_client& mqtt_client::operator=(mqtt_client other) 
    {
        swap(*this, other); 
        return *this;
    }


    bool mqtt_client::read_config()
    {
        beluga_core::comms::read_config();

        //TODO: CONFIG VAR FOR BUFFER SIZE
        _mqtt_client.setBufferSize(2048);


        std::string mqtt_client_name_key("mqtt_client_name");
        //std::string mqtt_client_name_val;
        bool mqtt_client_name_ok = _ini_ptr->get_config_value(_config_file_section, mqtt_client_name_key, &_mqtt_client_name);
        assert(mqtt_client_name_ok);

        std::string mqtt_username_key("mqtt_username"); 
        std::string mqtt_username_val;
        bool mqtt_username_ok = _ini_ptr->get_config_value(_config_file_section, mqtt_username_key, &_mqtt_username);
        assert(mqtt_username_ok);

        std::string mqtt_password_key("mqtt_password");
        std::string mqtt_password_val;
        bool mqtt_password_ok = _ini_ptr->get_config_value(_config_file_section, mqtt_password_key, &_mqtt_password);
        assert(mqtt_password_ok);

        std::string mqtt_server_address_key("mqtt_server_address");
        std::string mqtt_server_address_val;
        bool mqtt_server_address_ok = _ini_ptr->get_config_value(_config_file_section, mqtt_server_address_key, &mqtt_server_address_val);
        assert(mqtt_server_address_ok);

        //------------------------Optional fields---------------------------
        std::string mqtt_port_key("mqtt_server_port");
        std::string mqtt_port_val;
        _mqtt_port_number = 1883;
        bool mqtt_port_ok = _ini_ptr->get_config_value(_config_file_section, mqtt_port_key, &mqtt_port_val);
        if(mqtt_port_ok)
        {
            _mqtt_port_number = beluga_utils::string_to_int(mqtt_port_val);
        }




        std::string universal_topic_prefix_key("universal_topic_prefix");
        bool universal_topic_prefix_ok = _ini_ptr->get_config_value(_config_file_section, universal_topic_prefix_key, &_universal_topic_prefix);
        if(_universal_topic_prefix != "")
        {
            _use_universal_topic_prefix = true;
        }

        std::string mqtt_server_address_mode_key("mqtt_server_address_mode");
        std::string mqtt_server_address_mode_val;
        bool mqtt_server_address_mode_ok = _ini_ptr->get_config_value(_config_file_section, mqtt_server_address_mode_key, &mqtt_server_address_mode_val);

        _broker_address_mode = broker_address_mode::URL_STRING;
        if(mqtt_server_address_mode_ok)
        {
          std::string address_mode_str_lower = beluga_utils::string_to_lower(mqtt_server_address_mode_val);
           if( beluga_utils::string_contains_substring(address_mode_str_lower, "ip"))
           {
            _broker_address_mode = broker_address_mode::IP_ADDRESS_STRING;
           }
        }


        //-----Set MQTT broker server address-----
        std::vector<uint8_t> ip_address_vec;
        bool ip_str_ok;
        switch (_broker_address_mode)
        {
            case broker_address_mode::URL_STRING:
                _mqtt_server_address_str = mqtt_server_address_val;
                break;
            case broker_address_mode::IP_ADDRESS_STRING:
                ip_str_ok =  beluga_utils::string_to_ip_address_vec(mqtt_server_address_val, ip_address_vec);
                assert(ip_str_ok);
                //Load into IP Address struct
                for(int i = 0; i < 4; i++)
                {
                    _mqtt_server_ip[i] = ip_address_vec[i];
                }
                break;
            default:
                beluga_utils::debug_print_loop_forever("MQTT Client: Bad MQTT broker server address mode.");
                break;
        }
        
        //------------------Wifi config-------------
        //Read from environment variable
        //std::string wifi_network_key("wifi_network_name");
        //std::string wifi_password_key("wifi_password");
        //bool wifi_network_ok = _ini_ptr->get_config_value(_config_file_section, wifi_network_key, &_wifi_network_name );
        //bool wifi_password_ok = _ini_ptr->get_config_value(_config_file_section, wifi_password_key, &_wifi_password );
        _wifi_network_name = _WIFI_SSID;
        _wifi_password = _WIFI_PASSWORD;
        bool wifi_network_ok = _wifi_network_name != "";
        bool wifi_password_ok = _wifi_password != "";
        assert(wifi_network_ok);
        assert(wifi_password_ok);

        _mqtt_client.setClient(_wifi_client);
        if(_broker_address_mode == broker_address_mode::IP_ADDRESS_STRING)
        {
          _mqtt_client.setServer(_mqtt_server_ip, _mqtt_port_number);
        }else{
          _mqtt_client.setServer(_mqtt_server_address_str.c_str(), _mqtt_port_number);
        }
        _mqtt_client.setCallback(callback);

        WiFi.begin(_wifi_network_name.c_str(), _wifi_password.c_str());

    //https://github.com/knolleary/pubsubclient/blob/master/examples/mqtt_reconnect_nonblocking/mqtt_reconnect_nonblocking.ino
        return true;
    }

    bool mqtt_client::attempt_wifi_reconnect()
    {
        unsigned long time_now_ms = millis();
        //We force a wait of 5s between reconnects
        unsigned long dt_reconnect_ms = time_now_ms - _reconnect_attempt_time_ms;
        if (dt_reconnect_ms> 5000) {
            _reconnect_attempt_time_ms = time_now_ms;
            // Attempt to reconnect
            if (reconnect()) {
                _reconnect_attempt_time_ms = 0;

            beluga_utils::debug_print("");
            beluga_utils::debug_print("WiFi connected");
            beluga_utils::debug_print("IP address: ");
            _ss.str("");
            _ss << WiFi.localIP();
            beluga_utils::debug_print(_ss.str());
            _ss.str("");
            }
        }
        return _mqtt_client.connected();
    }
    
    bool mqtt_client::run(void * p )
    {
        if (!_mqtt_client.connected()) {
            Serial.println("Not connected!");
            attempt_wifi_reconnect();
            if(_mqtt_client.connected()){
                Serial.println("Connected!");
            }else{
                return false;
            }
        }
        //Run MQTT client. This will call the callback!
        _mqtt_client.loop();
        return true;

        #if 0
          if (!_mqtt_client.connected()) {
            Serial.println("Not connected!");
            attempt_wifi_reconnect();
            if(_mqtt_client.connected()){
                Serial.println("Connected!");
            }
          }else{
            //Serial.println("Connected!");
            //Load into outgoing
            run_tx();
            //Run MQTT client. This will call the callback!
            _mqtt_client.loop();
            //handle RX
            run_rx();
          }
          return true;
          #endif
    }

    
    bool mqtt_client::run_rx()
    {
        if (!_mqtt_client.connected()) {
            Serial.println("Not connected!");
            attempt_wifi_reconnect();
            if(_mqtt_client.connected()){
                Serial.println("Connected!");
            }else{
                return false;
            }
        }

        for(auto iter = mqtt_client::mqtt_callback_rx_data.begin(); iter != mqtt_client::mqtt_callback_rx_data.end(); iter++)
        {
            std::string topic_str = iter->first;
            std::string payload_str = iter->second;
            //For rx we delete the universal prefix.
            if(_use_universal_topic_prefix)
            {
                std::string prefix_deleted_str;
                bool prefix_deleted = beluga_utils::delete_prefix(topic_str, _universal_topic_prefix, prefix_deleted_str);
                if(prefix_deleted)
                {
                    topic_str = prefix_deleted_str;
                }
            }
            //Serial.println("Added to rx queue");
            //Serial.println(topic_str.c_str());
            add_to_rx_queue(payload_str, topic_str);
        }
        mqtt_client::mqtt_callback_rx_data.clear();
        return true;
    }


    bool mqtt_client::run_tx()
    {
        if (!_mqtt_client.connected()) {
            Serial.println("Not connected!");
            attempt_wifi_reconnect();
            if(_mqtt_client.connected()){
                Serial.println("Connected!");
            }else{
                return false;
            }
        }

            for(auto iter = _tx_queue.begin(); iter != _tx_queue.end(); iter++)
            {
                std::string topic_str = iter->first;
                if(topic_str == "")
                {
                    topic_str = beluga_utils::default_mqtt_tx_topic;
                }
                
                //For tx, we append the universal topic prefix
            if(_use_universal_topic_prefix)
            {
                _ss.str("");
                _ss << _universal_topic_prefix << topic_str;
                topic_str = _ss.str();
                _ss.str("");
            }
                //iter->second is a list of messages of type std::string
                for(auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
                {
                    _mqtt_client.publish(topic_str.c_str(), (*iter2).c_str());
                    Serial.print(topic_str.c_str());
                    Serial.print(" TX: ");
                    Serial.println((*iter2).c_str());
                }
                //EMpty the outbox
                iter->second.clear();
            }
        return true;
    }



}