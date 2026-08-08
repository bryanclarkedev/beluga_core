#pragma once



#pragma once
#include "beluga_device.h"
#include "Arduino.h"
#include <WiFi.h>

#include <vector>

namespace beluga_core
{
    class wifi_access_point : public beluga_core::device
    {
        public:
            bool configure_wifi_ap(std::string ip_addr_str, std::string gateway_str, std::string subnet_str);
            wifi_access_point(){};
            //Copy constructor
            wifi_access_point(const wifi_access_point &b) = default;
            //operator= is inherited from device as it is just a call to swap()
            //Swap for operator=
            friend void swap(wifi_access_point& first, wifi_access_point& second); 
            wifi_access_point& operator=(wifi_access_point other);
            virtual bool read_config();

        protected:
            //void set_static_ip(std::string ip_address_str);
            //bool str_to_ip_address(std::string ip_str, uint8_t * addr_array);
            //bool _ready_to_connect = false;
            std::string _wifi_ap_network_name = "";
            std::string _wifi_ap_password = "";
            //IPAddress * _server_ip_address;

            std::string _ap_ip_address_str = "";
            std::vector<uint8_t> _ap_ip_address_array;
            std::string _gateway_str = "";
             std::vector<uint8_t>  _gateway_array;
            std::string _subnet_str = "";
             std::vector<uint8_t>  _subnet_array; 
            //bool _connect_automatically;
            //uint16_t _port_number;

            //int16_t _max_connect_attempts = 10;
    };

}