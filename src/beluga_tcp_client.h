#pragma once

#include "beluga_comms.h"
#include "beluga_wifi_connection.h"



// Demonstration code to transfer data from server to client using TCP
// This code sends a 'request' in for form of a single character to the corresponding server, which responds with the value of Millis

//#include <Wifi.h>

#include <vector>
namespace beluga_core
{


    class tcp_client : public beluga_core::comms
    {
        public:
            bool initialise(std::string config_file_path, std::string config_file_section);
            bool connect_wifi();
            bool connect_client();
            bool connect();
            bool is_connected();
            bool has_rx();
            std::string get_rx();
            bool get_rx(std::string &);
            bool run_rx();
            bool run_tx();
            bool run(void * p = nullptr);
        protected:
            beluga_core::wifi_connection _wifi;//Wifi interface
            WiFiClient _client;//TCP client
            IPAddress * _server_ip_address;
            uint16_t _port_number; //We define a port number for comms between server and client
            std::string _wifi_client_name;
            std::stringstream _rx_ss;
            unsigned long _tx_time_ms = 0;
            unsigned long _rx_time_ms = 0;
            
    };
}

