#include "beluga_tcp_client.h"
//Run this with code from 
//https://forum.arduino.cc/t/link-to-a-very-basic-tutorial-about-tcp-evolved-to-tcp-socket-democode-to-send-receive-data-characters-similar-to-serial/949935/8
namespace beluga_core
{

    bool tcp_client::initialise(std::string config_file_path, std::string config_file_section)
    {
        //Perform initialisation as normal
        beluga_core::comms::initialise(config_file_path, config_file_section);
        //_wifi.initialise(config_file_path, config_file_section);

        beluga_utils::ini_reader ini(config_file_path);
        ini.initialise();
        std::string wifi_client_key("wifi_client_name");
        //std::string wifi_network_key("wifi_network_name");
        //std::string wifi_password_key("wifi_password");
          #if 0  
        beluga_utils::debug_print("TCP Client initialising....");
        bool wifi_client_ok =  ini.get_config_value(config_file_section, wifi_client_key, &_wifi_client_name );
        //bool wifi_network_ok = ini.get_config_value(config_file_section, wifi_network_key, &_wifi_network_name );
        //bool wifi_password_ok = ini.get_config_value(config_file_section, wifi_password_key, &_wifi_password );
        
        if(wifi_client_ok)
        {
            bool wifi_init = _wifi.initialise(config_file_path, _wifi_client_name);
        }else{
            beluga_utils::debug_print_loop_forever("tcp_client: Wifi client key not found in config!");
        }
#endif
        //IPAddress * _server_ip_address;
        //uint16_t _port_number;

        std::string server_ip_key("server_ip");
        std::string server_ip_str;
        bool server_ok = ini.get_config_value(config_file_section, server_ip_key, &server_ip_str );
        if(server_ok)
        {
            std::vector<std::string> ip_elements = beluga_utils::split_string(server_ip_str, ".");
            if(ip_elements.size() == 4)
            {
                std::vector<int> ip;
                for(auto i = ip_elements.begin(); i != ip_elements.end(); i++)
                {
                    ip.push_back(beluga_utils::string_to_int(*i));
                }
                _server_ip_address = new IPAddress(ip[0], ip[1], ip[2], ip[3]);
            }
        }else{
            beluga_utils::debug_print_loop_forever("tcp_client: server_ip key not found in config!");
        }

        std::string port_number_key("port_number");
        std::string port_number_str;
        bool port_number_ok = ini.get_config_value(config_file_section, port_number_key, &port_number_str );
        if(port_number_ok)
        {
            _port_number = beluga_utils::string_to_int(port_number_str);
        }else{
            beluga_utils::debug_print_loop_forever("tcp_client: port_number key not found in config!");
        }

        connect();

        return true;
    }

    bool tcp_client::is_connected()
    {
        //if(WiFi.status() != WL_CONNECTED)
        if  (!_wifi.is_connected())
        {
            return false;
        }
        return _client.connected();
    }

    bool tcp_client::connect()
    {
        bool wifi_ok =  WiFi.status() == WL_CONNECTED; //_wifi.connect(); //   connect_wifi();
        Serial.print("Wifi ok status: ");
        Serial.println(wifi_ok);
        if(wifi_ok)
        {
            return  connect_client();
        }
        return false;
    }

    bool tcp_client::run_rx()
    {
        bool got_rx = false;
        while(_client.available())
        {
            _rx_time_ms = millis();
            got_rx = true;
            _rx_ss << (char) _client.read();
        }
        return got_rx;

    }

    bool tcp_client::run_tx()
    {
        int FREQ_HZ = 1;
        if((millis() - _tx_time_ms) > (1000 / FREQ_HZ))
        {
            _tx_time_ms = millis();
            _client.print("A");
            return true;
        }
        return false;
    }

    bool tcp_client::run(void * p)
    {
        if(!_client.connected())
        {
            connect();
            return false;
        }
        run_rx();
        run_tx();

        return true;
    }

    bool tcp_client::has_rx()
    {
        return _rx_ss.str().size() > 0;
    }

    std::string tcp_client::get_rx()
    {
        if (_rx_ss.str().size() > 0)
        {
            std::string return_str = _rx_ss.str();
            _rx_ss.str("");
            return return_str;
        }
        return "";
    }



    bool tcp_client::connect_client()
    {
        if (!_client.connected()) {
            if (_client.connect(*_server_ip_address, _port_number)) {         // Connects to the server
                Serial.print("Connected to Gateway IP = "); 
                Serial.println(*_server_ip_address);
                return true;
            } else {
                Serial.print("Could NOT connect to Gateway IP = "); 
                Serial.println(*_server_ip_address);
                delay(500);
                return false;
            }
        }
        return true;
    }

}
