#pragma once
#include "beluga_device.h"
#include <Wifi.h>

#include <vector>

namespace beluga_core
{
    class wifi_connection : public device
    {
        public:
            bool read_config();
            bool connect();
            bool is_connected();
            bool run();
        protected:
            bool _ready_to_connect = false;
            std::string _wifi_network_name = "";
            std::string _wifi_password = "";
            bool _connect_automatically;
            uint16_t _port_number;

            int16_t _max_connect_attempts = 10;
    };
}