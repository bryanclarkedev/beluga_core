#pragma once
#include "beluga_mechanism.h"
//#include "extended_factory.h"
#include "beluga_ini_reader.h"
#include "beluga_factory.h"
#include "beluga_comms.h"
#include "beluga_machine.h"
#include "beluga_debug.h"
#include "beluga_thread.h"

namespace beluga_core
{
    /*!
    \brief 
    Nexus is an example of a machine intended to be a top-level class for bigger applications. 
    You have two threads, a nexus in each thread,
    and the nexus contains all the machines/devices/etc to run everything.

    Nexus reads from the rx_buffer and appends to the tx_buffer as part of its run() function.

    */
    class nexus : public machine
    {
        public:
            nexus(){};

            virtual bool read_config(){
                std::string config_val;
                
                bool config_ok = false;

                config_ok = machine::read_config();
                if(! config_ok)
                {
                    _ss.str("");
                    _ss << "Could not read nexus config for device " << _device_name;
                    beluga_utils::debug_print_loop_forever(_ss.str());
                }
                //-----Comms period is optional----
                std::string comms_period_ms_key("comms_period_ms"); 
                std::string comms_period_ms_val_str;
                _comms_period_ms = 1000;
                bool comms_period_ok = _ini_ptr->get_config_value(_config_file_section, comms_period_ms_key, &comms_period_ms_val_str);
                if(comms_period_ok){
                    _comms_period_ms = beluga_utils::string_to_int(comms_period_ms_val_str);
                }

                return true;
            }
            //
            virtual bool run(void * p = nullptr){

                //void pointer casting like this can be a bit dicey 
                //but we know that the pointer is to the parent thread
                beluga_core::thread * parent_thread =  static_cast<beluga_core::thread *>(p); 
                std::shared_ptr<beluga_core::interthread_buffer> rx_buffer = parent_thread->_rx_buffer;
                std::shared_ptr<beluga_core::interthread_buffer> tx_buffer = parent_thread->_tx_buffer;
                bool buffers_ok = (rx_buffer != nullptr) && (tx_buffer != nullptr);
                if(! buffers_ok){
                    beluga_utils::debug_print_loop_forever("Error in nexus getting threads");
                }else{
                    //We read from the _rx_buffer's internal list of messages and print to serial
                    std::string this_msg;
                    while(rx_buffer->get_rx_msg(this_msg))
                    {
                        this->msg_thread_buffer_to_nexus(this_msg);
                    }
                    //Run base class (which runs subdevices)
                    beluga_core::machine::run(p);
                    generate_report_string();

                    //We will dump some placeholder messages to the _tx_buffer.
                    unsigned long time_now_ms = millis();
                    unsigned long dt_ms = time_now_ms - _time_ms;
                    if(dt_ms >= _comms_period_ms )
                    {
                        std::list<std::string> msg_list;
                        bool got_msg_out = msg_nexus_to_thread_buffer(msg_list);                        
                        if(got_msg_out)
                        {
                            for(auto iter = msg_list.begin(); iter != msg_list.end(); iter++)
                            {
                                //_ss.str("");
                                //_ss << _device_name << ": " << millis() << "!";
                                tx_buffer->add_to_tx_queue(*iter);
                                //_ss.str("");
                                //Serial.println(_report_string.c_str());
                            }
        
                        }
                        _time_ms = millis();
                    }
                }
                return true;
            }

            //Reimplement this as needed
            virtual bool msg_thread_buffer_to_nexus(std::string in_msg){
                _ss.str("");
                _ss << _device_name  << " Nexus got msg in: '" << in_msg << "'";
                beluga_utils::debug_print(_ss.str());
                _ss.str("");
                return true;
            }
            
            //Reimplement this as needed
            virtual bool msg_nexus_to_thread_buffer(std::list<std::string> & out_msg_list){
                out_msg_list.clear();
                if(_report_string != "" ){
                    out_msg_list.push_back(_report_string);
                } 
                return out_msg_list.size() > 0;
            }


        protected:
            int _comms_period_ms = 1000;

    };

}