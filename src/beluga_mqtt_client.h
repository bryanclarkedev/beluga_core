// Demonstration code to transfer data from server to client using TCP
// This code sends a 'request' in for form of a single character to the corresponding server, which responds with the value of Millis
#pragma once
#include "beluga_comms.h"

//#include "beluga_wifi_connection.h"


//#include <Wifi.h>

#include <vector>
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

