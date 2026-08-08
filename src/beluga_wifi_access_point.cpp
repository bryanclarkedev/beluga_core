#include "beluga_wifi_access_point.h"

//#include "wifi_access_point.h"
namespace beluga_core {


    //bool wifi_access_point::initialise(std::string config_file_path, std::string _config_file_section)
    bool wifi_access_point::read_config()
    {
        //Perform initialisation as normal
        //Beluga_Origin::initialise(config_file_path, _config_file_section);

        //Beluga_Ini_Reader ini(config_file_path);
        //ini.initialise();
        std::string wifi_ap_network_key("wifi_ap_ssid");
        std::string wifi_ap_password_key("wifi_ap_password");
    
        Serial.println("Wifi AP initialising....");

        bool wifi_ap_network_ok = _ini_ptr->get_config_value(_config_file_section, wifi_ap_network_key, &_wifi_ap_network_name );
        bool wifi_ap_password_ok = _ini_ptr->get_config_value(_config_file_section, wifi_ap_password_key, &_wifi_ap_password );
        if((! wifi_ap_network_ok ) || (! wifi_ap_password_ok))
        {
            beluga_utils::debug_print_loop_forever("Error in wifi_access_point: problem with wifi network name or password in config");
        }
        const char * ap_ssid_name = _wifi_ap_network_name.c_str();
        const char * ap_password = _wifi_ap_password.c_str();
        WiFi.mode(WIFI_AP); 
        WiFi.softAP(ap_ssid_name, ap_password);

        //======See if IP address is set
        std::string ip_address_key("ip_address");
        bool ip_address_ok = _ini_ptr->get_config_value(_config_file_section, ip_address_key, &_ap_ip_address_str );
        if(! ip_address_ok)
        {
            //IP address not specified.
            //Get the default IP address
            IPAddress myIP = WiFi.softAPIP();
            _ap_ip_address_str = std::string(myIP.toString().c_str());
            _ss.str("");
            _ss << "AP IP address: " << _ap_ip_address_str;
            beluga_utils::debug_print(_ss.str());
            _ss.str("");
            return true;
        }
        //IP address set, get gateway and subnet
        #if 0
        std::string gateway_key("gateway");//Assume this will be the same as ip_address
        bool gateway_ok = ini.get_config_value(_config_file_section, gateway_key, &_gateway_str );
        if(! gateway_ok)
        {
            //Gateway not set, use ip address
            _gateway_str = _ap_ip_address_str;
        }
        #endif
        bool gateway_ok = ip_address_ok;
        _gateway_str = _ap_ip_address_str;

        std::string subnet_key("subnet");
        bool subnet_ok = _ini_ptr->get_config_value(_config_file_section, subnet_key, &_subnet_str );
        if(! subnet_ok)
        {
            //Use subnet IP
            _subnet_str = "255.255.255.0";
        }

        return  configure_wifi_ap(_ap_ip_address_str, _gateway_str, _subnet_str);
    }

    /*
    If the config file includes static IP, gateway and subnet, this will be called in initialise()
    Otherwise it can be called later.
    */
    bool wifi_access_point::configure_wifi_ap(std::string ip_addr_str, std::string gateway_str, std::string subnet_str)
    {
        /*
        IPAddress * ip = self. beluga_utils::string_to_ip_address_vec(ip_addr_str);
        IPAddress * gateway = self. beluga_utils::string_to_ip_address_vec(gateway_str);
        IPAddress * subnet = self. beluga_utils::string_to_ip_address_vec(subnet_str);
        */

    bool ip_ok =  beluga_utils::string_to_ip_address_vec(ip_addr_str, _ap_ip_address_array);
    bool gateway_ok =  beluga_utils::string_to_ip_address_vec(gateway_str, _gateway_array);
    bool subnet_ok =  beluga_utils::string_to_ip_address_vec(subnet_str, _subnet_array);
    IPAddress ip(_ap_ip_address_array[0], _ap_ip_address_array[1], _ap_ip_address_array[2], _ap_ip_address_array[3]);
    IPAddress gateway(_gateway_array[0], _gateway_array[1], _gateway_array[2], _gateway_array[3]);
    IPAddress subnet(_subnet_array[0], _subnet_array[1], _subnet_array[2], _subnet_array[3]);

        WiFi.softAPConfig(ip, gateway, subnet);


        return true;
    }

    #if 0
    bool wifi_access_point:: beluga_utils::string_to_ip_address_vec(std::string ip_str, uint8_t * addr_array)
    {
        std::vector<std::string> ip_elements = split_string(ip_str, ".");
        if(ip_elements.size() == 4)
        {
            for(int i = 0; i < 4; i++)
            {
                addr_array[i] = stoi(ip_elements[i]);
            }
            #if 0
            std::vector<int> ip;
            for(auto i = ip_elements.begin(); i != ip_elements.end(); i++)
            {
                ip.push_back(string_to_int(*i));
            }
            addr = new IPAddress(ip[0], ip[1], ip[2], ip[3]);
            #endif
            return true;
        }
        return false;
    }
    #endif

    #if 0
    void wifi_access_point::set_static_ip(std::string ip_address_str)
    {
        IPAddress * local_ip;
        bool ip_ok =  beluga_utils::string_to_ip_address_vec(ip_address_str, local_ip);
        //https://lastminuteengineers.com/creating-esp32-web-server-arduino-ide/
        WiFi.softAPConfig(local_ip, gateway, subnet);
    }
    #endif



}