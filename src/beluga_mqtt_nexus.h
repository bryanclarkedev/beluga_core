#pragma once
#include "beluga_nexus.h"
#include "beluga_mqtt_client.h"

#include "beluga_constants.h"

namespace beluga_core
{
    /*!
    \brief 
    Nexus is an example of a machine intended to be a top-level class for bigger applications. 
    You have two threads, a nexus in each thread,
    and the nexus contains all the machines/devices/etc to run everything.

    Nexus reads from the rx_buffer and appends to the tx_buffer as part of its run() function.

    */
    class mqtt_nexus : public nexus
    {
        public:
            mqtt_nexus(){};

            virtual bool read_config(){
                beluga_core::nexus::read_config();
                _mqtt_client_name = "";
                for(int i = 0; i < _subdevice_types.size(); i++){
                    //TODO: 
                    bool is_mqtt_client = _subdevice_types[i] == Enum2String(beluga_core_object_enum::mqtt_client);
                    if(! is_mqtt_client){
                        continue;
                    }
                    _mqtt_client_name = _subdevice_names[i];
                }
                bool no_mqtt_client = _mqtt_client_name == "";
                if(no_mqtt_client){
                    beluga_utils::debug_print_loop_forever("mqtt_nexux error: could not find subdevice of type mqtt_client!");
                }


                std::shared_ptr<beluga_core::device> this_device;
                bool got_subdevice = get_subdevice(_mqtt_client_name, this_device);
                if(! got_subdevice)
                {
                    return false;
                }
                _mqtt_client_ptr = std::static_pointer_cast<beluga_core::mqtt_client>(this_device);


                return true;
            }
            
            //Reimplement this as needed
            virtual bool msg_thread_buffer_to_nexus(std::string in_msg){
                _ss.str("");
                _ss << _device_name  << " mqtt_nexus got msg in: '" << in_msg << "'";
                beluga_utils::debug_print(_ss.str());
                _ss.str("");
                //Publish
                std::shared_ptr<beluga_core::device> this_device;

                _mqtt_client_ptr->add_to_tx_queue(in_msg.c_str(), "beluga/esp32/mqtt_nexus_tx");                
                return true;
            }
            
            //Reimplement this as needed
            virtual bool msg_nexus_to_thread_buffer(std::list<std::string> & out_msg_list){
                /*
                if(_report_string == "" ){
                    return false;
                } 
                out_msg = _report_string;
                */
               for(auto topic_iter = _mqtt_client_ptr->_rx_topic_list.begin(); topic_iter !=  _mqtt_client_ptr->_rx_topic_list.end(); topic_iter++)
               {
                std::list<std::string> this_mail;
                _mqtt_client_ptr->get_rx_queue(this_mail, *topic_iter);  
                for(auto mail_iter =  this_mail.begin(); mail_iter != this_mail.end(); mail_iter++){
                    _ss.str("");
                    _ss << *topic_iter << beluga_utils::concatenation_delimiter << *mail_iter;
                    out_msg_list.push_back(_ss.str());
                    _ss.str("");
                }

               }
            //std::string this_topic_str = "beluga/esp32/mqtt_nexus_rx";
            //_mqtt_client_ptr->get_rx_queue(out_msg_list, this_topic_str);  
            return out_msg_list.size() > 0;
            
            }



        protected:
            std::shared_ptr<mqtt_client> _mqtt_client_ptr;
            std::string _mqtt_client_name = "";
    };

}