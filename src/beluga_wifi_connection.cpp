#include "beluga_wifi_connection.h"
#include "beluga_debug.h"
namespace beluga_core
{
    /*
    [wifi_connection]
    wifi_network_name = my_wifi
    wifi_password = password1
    connect_automatically = true
    */
    bool wifi_connection::read_config()
    {
        std::string wifi_network_key("wifi_network_name");
        std::string wifi_password_key("wifi_password");
        std::string connect_automatically_key("connect_automatically");
        std::string connect_automatically_str;
        std::string max_connect_attempts_key("max_connection_attempts");
        std::string max_connect_attempts_str;
        beluga_utils::debug_print("Wifi initialising....");

        bool wifi_network_ok = _ini_ptr->get_config_value(_config_file_section, wifi_network_key, &_wifi_network_name );
        bool wifi_password_ok = _ini_ptr->get_config_value(_config_file_section, wifi_password_key, &_wifi_password );
        bool connect_automatically_ok = _ini_ptr->get_config_value(_config_file_section, connect_automatically_key, &connect_automatically_str );
        bool max_connection_attempts_ok = _ini_ptr->get_config_value(_config_file_section, max_connect_attempts_key, &max_connect_attempts_str );
    
    if(max_connection_attempts_ok)
        {
            _max_connect_attempts = stoi(max_connect_attempts_str);
        }else{
            _max_connect_attempts = 10;
        }
        if(connect_automatically_ok)
        {
            _connect_automatically = beluga_utils::string_to_bool(connect_automatically_str);
        }else{
            _connect_automatically = false;
        }

        _ready_to_connect = (wifi_network_ok && wifi_password_ok);
        if(! _ready_to_connect)
        {
            beluga_utils::debug_print_loop_forever("Error with wifi_connection config: wifi_network_name and/or wifi_password");
        }

        if(! _connect_automatically)
        {
            return true;
        }

        return connect();
        
    }

    bool wifi_connection::connect()
    {
        if(!_ready_to_connect)
        {
            return false;
        }
        bool already_connected = (WiFi.status() == WL_CONNECTED);
        if(already_connected)
        {
            return true;
        }
        int max_tries = _max_connect_attempts;
        int tries_tally = 0;
        WiFi.mode(WIFI_STA);
        WiFi.begin(_wifi_network_name.c_str(), _wifi_password.c_str());


        while (WiFi.status() != WL_CONNECTED) {
        delay(250);
        tries_tally++;
        beluga_utils::debug_print("Wifi could not connect...");
        if(tries_tally >= max_tries)
        {
           beluga_utils::debug_print("Wifi is not working!!!!!!!!!");
            return false;
        }
        }
        
    beluga_utils::debug_print("");
    beluga_utils::debug_print("WiFi connected");
    beluga_utils::debug_print("IP address: ");
    _ss.str("");
    _ss << WiFi.localIP();
    beluga_utils::debug_print(_ss.str());
    _ss.str("");
        return true;
    }

    bool wifi_connection::is_connected()
    {
        bool ok = (WiFi.status() == WL_CONNECTED);
        if(ok)
        {
            return true;
        }
        if(_connect_automatically)
        {
            return connect();
        }
        return false;
    }


    bool wifi_connection::run()
    {
        return is_connected();
    }

}