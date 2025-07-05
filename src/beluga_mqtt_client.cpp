#include "beluga_mqtt_client.h"
#include <WiFi.h>
#include <PubSubClient.h>

//#include <WiFi.h>
//#include <PubSubClient.h>


WiFiClient espWifiClient;
PubSubClient client(espWifiClient);


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



boolean nonblock_reconnect(std::string client_name, std::string mqtt_username, std::string mqtt_password, std::vector<std::string> rx_topics_list, std::string rx_topics_prefix) {
    
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect    
    if (client.connect(client_name.c_str(), mqtt_username.c_str(), mqtt_password.c_str())) {
     //if (client.connect(client_name.c_str())) {

      Serial.println("MQTT connected");
      
      for(int i = 0; i < rx_topics_list.size(); i++)
      {
        std::string this_topic = rx_topics_list[i];
        if(rx_topics_prefix != "")
        {
          std::stringstream ss;
          ss << rx_topics_prefix << this_topic;
          this_topic = ss.str();
        }
        Serial.print(i);
        Serial.print(": ");
        Serial.println(this_topic.c_str());
        client.subscribe(this_topic.c_str());
      }
      // Subscribe - moved to outside function
      std::string subscription_topic_name = rx_topics_list[0];// "esp32/output";
      //Serial.print("SUBSCRIBE TO: ");
      //Serial.println(subscription_topic_name.c_str());
      //client.subscribe(subscription_topic_name.c_str());
      //start_subscriptions()

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in a few seconds");
  }
  return client.connected();
}


void mqtt_client::reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT re-connection...");
    // Attempt to connect    
    if (client.connect(_mqtt_client_name.c_str(), _mqtt_username.c_str(), _mqtt_password.c_str())) {
      Serial.println("connected");
      // Subscribe
      std::string subscription_topic_name = "esp32/output";
      client.subscribe(subscription_topic_name.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void mqtt_client::send_to_mqtt(std::string topic, std::string payload)
{
    add_to_tx_queue(payload, topic); //Note the order of arguments.
}


bool mqtt_client::run(void *)
{
   if (!client.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (nonblock_reconnect(_mqtt_client_name, _mqtt_username, _mqtt_password, _rx_topic_list, _master_topic_prefix)) {
        lastReconnectAttempt = 0;
      }
    }
  }else{
    //Serial.println("CONNECTED!");
    //Cient connected
    client.loop(); //This runs tx/rx stuff.
    run_rx();
    run_tx();
  }
  return true;
}


bool mqtt_client::run_rx()
{
    for(auto iter = mqtt_client::mqtt_callback_rx_data.begin(); iter != mqtt_client::mqtt_callback_rx_data.end(); iter++)
    {
        std::string topic_str = iter->first;
        std::string payload_str = iter->second;
        //For rx we delete the master prefix.
        if(_use_master_topic_prefix)
        {
          std::string prefix_deleted_str;
          bool prefix_deleted = beluga_utils::delete_prefix(topic_str, _master_topic_prefix, prefix_deleted_str);
          if(prefix_deleted)
          {
            topic_str = prefix_deleted_str;
          }
        }

        add_to_rx_queue(payload_str, topic_str);
    }
    mqtt_client::mqtt_callback_rx_data.clear();
    return true;
}


bool mqtt_client::run_tx()
{
        for(auto iter = _tx_queue.begin(); iter != _tx_queue.end(); iter++)
        {
            std::string topic_str = iter->first;
            if(topic_str == "")
            {
                topic_str = _default_tx_topic;
            }
            
            //For tx, we append the master topic prefix
           if(_use_master_topic_prefix)
           {
            _ss.str("");
            _ss << _master_topic_prefix << topic_str;
            topic_str = _ss.str();
            _ss.str("");
           }
            //iter->second is a list of messages of type std::string
            for(auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++)
            {
                client.publish(topic_str.c_str(), (*iter2).c_str());
                Serial.print(topic_str.c_str());
                Serial.print(" TX: ");
                Serial.println((*iter2).c_str());
            }
            //EMpty the outbox
            iter->second.clear();
        }
      return true;
}

    //copy-and-sep for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    void swap(mqtt_client& first, mqtt_client& second) // nothrow
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        swap(static_cast<comms &>(first), static_cast<comms &>(second));

        //swap(first._pin, second._pin);   
    }

    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    mqtt_client& mqtt_client::operator=(mqtt_client other) 
    {
        swap(*this, other); 
        return *this;
    }



    /*
    wifi_connection_name = wifi_connection1
    mqtt_client_name = mqtt_client1
    */
    bool mqtt_client::read_config()
    {
        comms::read_config();

        #if False
        std::string wifi_network_ssid_key("wifi_network_name");
        std::string wifi_network_ssid_val;
        bool wifi_network_ssid_ok = _ini_ptr->get_config_value(_config_file_section, wifi_network_ssid_key, &_wifi_ssid);

        std::string wifi_network_password_key("wifi_password");
        std::string wifi_network_password_val;
        //bool wifi_network_password_ok = _ini_ptr->get_config_value(_config_file_section, wifi_network_password_key, &wifi_network_password_val);
        bool wifi_network_password_ok = _ini_ptr->get_config_value(_config_file_section, wifi_network_password_key, &_wifi_password);
        #endif

        std::string mqtt_client_name_key("mqtt_client_name");
        //std::string mqtt_client_name_val;
        bool mqtt_client_name_ok = _ini_ptr->get_config_value(_config_file_section, mqtt_client_name_key, &_mqtt_client_name);

        std::string mqtt_username_key("mqtt_username"); 
        std::string mqtt_username_val;
        bool mqtt_username_ok = _ini_ptr->get_config_value(_config_file_section, mqtt_username_key, &_mqtt_username);

        std::string mqtt_password_key("mqtt_password");
        std::string mqtt_password_val;
        bool mqtt_password_ok = _ini_ptr->get_config_value(_config_file_section, mqtt_password_key, &_mqtt_password);

        std::string mqtt_server_address_key("mqtt_server_address");
        std::string mqtt_server_address_val;
        bool mqtt_server_address_ok = _ini_ptr->get_config_value(_config_file_section, mqtt_server_address_key, &mqtt_server_address_val);

        //Different processing for IP address vs web address
        std::vector<std::string> split_str_vals = beluga_utils::split_string(mqtt_server_address_val, ".");
        std::vector<uint8_t> ip_address_vec;
        bool _use_server_ip = beluga_utils::string_to_ip_address_vec(mqtt_server_address_val, ip_address_vec);
        if(_use_server_ip)
        {
          Serial.println(">>Using IP address");
          for(int i = 0; i < 4; i++)
          {
            _mqtt_server_ip[i] = ip_address_vec[i];
          }
        }else{
            Serial.println(">>Using web address");
          _mqtt_server_address = mqtt_server_address_val;
        }

        std::string mqtt_default_tx_topic("mqtt_default_tx_topic");
        std::string mqtt_default_tx_topic_val;
        bool mqtt_default_tx_topic_ok =  _ini_ptr->get_config_value(_config_file_section, mqtt_default_tx_topic, &_default_tx_topic);
        
        std::string master_topic_prefix_key("master_topic_prefix");
        bool master_topic_prefix_ok = _ini_ptr->get_config_value(_config_file_section, master_topic_prefix_key, &_master_topic_prefix);
        if(_master_topic_prefix != "")
        {
            _use_master_topic_prefix = true;
        }

        bool mqtt_configured_ok = mqtt_client_name_ok && mqtt_username_ok && mqtt_password_ok && mqtt_server_address_ok;
        if(!mqtt_configured_ok)
        {
            beluga_utils::debug_print_loop_forever("Error in mqtt_client: one of the config variables failed");
        }
        #if 0
        if(wifi_network_ssid_ok && wifi_network_password_ok)
        {
          _manage_wifi_here = true;
          Serial.print("Set wifi password to: ");
          Serial.println(_wifi_password.c_str());
          setup_wifi();
        }
        #endif
        if(_use_server_ip)
        {
          client.setServer(_mqtt_server_ip, 1883);
        }else{
          client.setServer(_mqtt_server_address.c_str(), 1883);
        }
        client.setCallback(mqtt_client::callback);

        return true;
    }

    #if 0
    bool mqtt_client::is_connected()
    {
        //if(WiFi.status() != WL_CONNECTED)
        if  (!_wifi.is_connected())
        {
            return false;
        }
        return _mqtt_client.connected();
    }

    bool mqtt_client::connect()
    {
        bool wifi_ok = _wifi.connect(); //   connect_wifi();
        Serial.print("Wifi ok status: ");
        Serial.println(wifi_ok);
        if(!wifi_ok)
        {
            return false;
         }
        return  connect_to_server();
    }

    bool mqtt_client::connect_to_server()
    {
        if (!_mqtt_client.connected()) 
        {
            if (_mqtt_client.connect(*_server_ip_address, _port_number)) 
            {         
                // Connects to the server
                Serial.print("mqtt_client: connected to Gateway IP = "); 
                Serial.println(*_server_ip_address);
                return true;
            } else {
                Serial.print("mqtt_client: could NOT connect to Gateway IP = "); 
                Serial.println(*_server_ip_address);
                delay(500);
                return false;
            }
        }
        return true;
    }
    #endif

}