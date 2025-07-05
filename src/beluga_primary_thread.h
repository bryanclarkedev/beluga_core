#pragma once

#include "beluga_thread.h"
#include "beluga_machinery.h"
#include "beluga_debug.h"
/*

Thread is a type of machine
Machine has _subdevices
This thread is a SPECIAL type of machine in that it has only ONE subdevice.
The one subdevices is of a class that inherits from beluga_core::machinery.
This one device is called the PRIMARY DEVICE. Think of it as the BIGGEST box containing all the little boxes of stuff for this thread.



The primary device can have as many sub-subdevices/sub-comms, etc, as it wants. But the top level object is the ONLY ONE i.e. 
in the config for this thread, there is only one item in the _subdevices list and nothing in the comms list.

In pre_run the messages from the interthread buffer got dumped into the rx interthread_buffer's _rx_queue. _rx_queue is a map<topic_str, payload_str> but they get added in under topic "".
We iterate over the rx buffer's _rx_queue
- handle those messages in run()
- publish any outputs to the other thread in post_run()

This is a template class
The templated type is the class of primary device.


IF you want to run an object in a thread cleanly, make a machinery class for it (i.e. a wrapper around it of type machinery and implementations of read_config() and run() as needed. You may just need run() as read_config is not changed.
*/
namespace beluga_core
{
        class primary_thread : public thread
        {
            public:
                virtual bool read_config()
                {
                    beluga_core::thread::read_config(); 

                    bool _machinery_in_subdevices = false;

                    if((_subdevice_names.size() != 1) || (_comms_names.size() != 0))
                    {
                           beluga_utils::debug_print_loop_forever("primary_thread error: there can be only one subdevice in the config!");
                    }

                    _primary_device_name = _subdevice_names[0];
                    Serial.print("primary device name: ");
                    Serial.println(_primary_device_name.c_str());
                    //Read the config section for the subdevice (i.e. the primary device) to get its type
                    std::string device_type_config_key("device_type");
                    bool config_ok = _ini_ptr->get_config_value(_primary_device_name, device_type_config_key, &_primary_device_type );
                    if(!config_ok)
                    {
                        beluga_utils::debug_print_loop_forever("primary_thread error: Could not get primary device type");
                    }    

                    std::shared_ptr<beluga_core::device> this_device;
                    bool primary_device_ptr_ok = get_subdevice(_primary_device_name, this_device);
                    if(! primary_device_ptr_ok)
                    {
                        beluga_utils::debug_print_loop_forever("primary_thread error: could not get pointer to primary device");
                    }
                    _primary_device_ptr = std::static_pointer_cast<beluga_core::machinery>(this_device);
                   //This line breaks
                    // _primary_device_ptr->set_parent(static_cast<beluga_core::device>(this)); //Set the parent of the primary device in case it wants to do stuff.
                    return true;
                }
            protected:
                //bool mqtt_handle_rx_from_other_thread(std::string this_rx_str);
                virtual bool pre_run(void * params = nullptr)
                {
                    bool interthread_rx_buffer_ok = beluga_core::thread::pre_run(); //Will run the _rx_buffer to receive from the other thread

                    if(! interthread_rx_buffer_ok)
                    {
                        return false;
                    }
            

                    std::string this_msg;
                    std::list<std::string> interthread_rx_list;
                    bool at_least_one_rx = _rx_buffer->get_rx_queue(interthread_rx_list);

                    #if 0
                    bool at_least_one_rx = false;
                    while(_rx_buffer->get_rx_msg(this_msg)) //_rx_buffer is the interthread rx buffer
                    {
                        interthread_rx_list.push_back(this_msg);
                        at_least_one_rx = true;
                    }
                    #endif
                    //Pass data from _rx_buffer down to _primary_device topic ""
                    if(at_least_one_rx)
                    {
                        _ss.str("");
                        _ss << _task_name << " thread rx: " << this_msg;
                        debug_print(_ss.str());
                        _primary_device_ptr->add_rx(interthread_rx_list); //Add to machinery's primary rx
                    }
                    
                    return true;
                }

                virtual bool post_run(void * params = nullptr)
                {
                    if(_tx_buffer == nullptr)
                    {
                        _ss.str("");
                        _ss << _task_name << " no tx buffer!!!";
                        debug_print(_ss.str());
                        return false;
                    }

                    //Forward machinery tx to interprocess thread tx
                    std::list<std::string> machinery_tx_list;
                    
                    bool got_machinery_tx_ok = _primary_device_ptr->get_tx(machinery_tx_list); 
                    if(got_machinery_tx_ok)
                    {
                        for(auto iter = machinery_tx_list.begin(); iter != machinery_tx_list.end(); iter++)
                        {
                            //Add to the tx interthread buffer
                            Serial.print("Machinery sending to tx buff: ");
                            Serial.println(iter->c_str());
                            _tx_buffer->add_to_tx_queue(*iter); //_tx_buffer is the interthread tx buffer
                        }
                    }
                    
                    
                    return beluga_core::thread::post_run(); //Will run the _tx_buffer to transmit to the other thread

                }

                virtual bool run(void * params = nullptr)
                {
                    _primary_device_ptr->run(params);
                    delay(1); //This delay is critical to stop the watchdog timer from timing out. Do not delete it.
                    return false;
                }

                std::string _primary_device_name = "";
                std::string _primary_device_type = "";
                std::shared_ptr<beluga_core::machinery> _primary_device_ptr = nullptr;
        };

}