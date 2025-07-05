#pragma once
#include "beluga_machinery.h"
#include "beluga_mqtt_client.h"


/*
This is a wrapper around a mqtt client (and other objects)
machinery provides a 'top-level' comms object with an external interface which 
In this case the mqtt client communicates with some external object(s)
*/
namespace beluga_core
{
    #if 0
    class mqtt_client_machinery : public machinery
    {
        public:
            mqtt_client_machinery(){};
            virtual bool run(void * p = nullptr);
            virtual bool read_config();

            virtual bool handle_primary_rx();//std::vector< std::map<std::string, std::string> > & rx_subtopic_map_vec);
            virtual bool generate_primary_tx();

            //Sometimes we use pre-existing comms
            //virtual bool set_comms(std::map<std::string, std::shared_ptr<beluga_core::comms> > & comms_map);
            //bool get_comms(std::string s,  std::shared_ptr<beluga_core::comms> & return_val);
            std::string get_mqtt_client_name(){return _mqtt_client_name;}
        private:
            std::string _mqtt_client_name = "";
            std::shared_ptr<beluga_core::mqtt_client> _mqtt_ptr = nullptr;

            std::vector<std::string> _mqtt_rx_topics;

            
    };
    #endif
}