#pragma once
#include "beluga_comms.h"
#include <vector>
#include <PubSubClient.h>
#include <WiFi.h>

/*
Assume wifi is already connected
*/

namespace beluga_core
{
    enum broker_address_mode { IP_ADDRESS_STRING, URL_STRING };
    class mqtt_client : public comms
    {
        public:
            //operator=
            mqtt_client& operator=(mqtt_client other);
            //Swap for operator=
            friend void swap(mqtt_client& first, mqtt_client& second); 

            bool read_config();
            bool connect();
            bool reconnect();
            bool is_connected();
            bool run_rx();
            bool run_tx();
            bool run(void * p = nullptr);
            static std::list< std::pair<std::string, std::string> > mqtt_callback_rx_data;
            static void callback(char* topic, byte* message, unsigned int length); //Must be static

        protected:
            bool attempt_wifi_reconnect();
            bool parse_ip_address_str(std::string ip_address_str, std::vector<uint8_t> ip_address_vec);
            unsigned long _tx_time_ms = 0;
            unsigned long _rx_time_ms = 0;
            unsigned long _reconnect_attempt_time_ms = 0;
            //-------------------------MQTT broker parameters---------------
            std::string _mqtt_client_name; //This should be unique or you'll get random disconnections
            std::string _mqtt_username; //For user authentication
            std::string _mqtt_password; //For user authentication
            uint16_t _mqtt_port_number = 1883;

            std::string _universal_topic_prefix = "";
            bool _use_universal_topic_prefix = false;

            //MQTT broker address is either an IP address or URL string
            broker_address_mode _broker_address_mode = broker_address_mode::URL_STRING; //If true, use _mqtt_server_ip e.g. 192.123.4.5. If false use _mqt_server_address e.g. "www.mymqtt.com"

            std::string _mqtt_server_address_str;
            IPAddress _mqtt_server_ip;

            std::string _wifi_network_name = "";
            std::string _wifi_password = "";

            WiFiClient _wifi_client;
            PubSubClient _mqtt_client;
    };
}

#if 0
boolean nonblock_reconnect(std::string client_name, std::string mqtt_username, std::string mqtt_password, std::vector<std::string> rx_topics_list, std::string rx_topics_prefix = "");

namespace beluga_core
{
    class mqtt_client : public comms
    {
        static void callback(char* topic, byte* message, unsigned int length) ;
        // LED Pin
       std::string _mqtt_client_name;
       std::string _wifi_password;
       std::string _wifi_ssid;
       std::string _mqtt_username;
       std::string _mqtt_password;
       std::string _mqtt_server_address;
       IPAddress _mqtt_server_ip;
       bool _use_server_ip;
       
       static std::list< std::pair<std::string, std::string> > mqtt_callback_rx_data;

       public:
            mqtt_client(){};
            mqtt_client(const mqtt_client &b) = default;
            //operator=
            mqtt_client& operator=(mqtt_client other);
            //Swap for operator=
            friend void swap(mqtt_client& first, mqtt_client& second); 

            //bool initialise(std::string config_file_path, std::string config_file_section);
            bool read_config();
            //bool connect_wifi();
            //bool connect_to_server();
            bool connect();
            bool is_connected();
            bool run_rx();
            bool run_tx();
            bool run(void * p = nullptr);
            void reconnect();
           // void setup_wifi();

            //void start_subscriptions();
            void send_to_mqtt(std::string topic, std::string payload);

        protected:
          
            unsigned long _tx_time_ms = 0;
            unsigned long _rx_time_ms = 0;

            char msg[50];
            int value = 0;

            unsigned long lastReconnectAttempt = 0;

            bool _manage_wifi_here = false;
            std::string _default_tx_topic = "beluga/mqtt";
            bool _use_master_topic_prefix = false;
            std::string _master_topic_prefix = "";


    };
}

    #endif

