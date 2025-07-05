#pragma once

//#include <string>
//#include <sstream>
//#include "Beluga_Exception.h"
#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"

#include "beluga_comms.h"
/*
This class is a wrapper around a FreeRTOS ring buffer.
It is used to pass data between threads. 

A typical setup will have two threads (one per ESP32 core) and two interthread buffers (1->2 and 2->1)
*/
namespace beluga_core
{
    class interthread_buffer : public beluga_core::comms
    {
        public:
            //interthread_buffer();
            //bool initialise(std::string config_file_path, std::string config_file_section);
            bool read_config();
            bool run_rx();
            bool run_tx();
            //void transmit_to_buffer(std::string s);
            //bool receive_from_buffer(std::string * s);
            
        protected:
            uint16_t _buffer_size = 4096; //This is the default size but it can be set in the config
            RingbufHandle_t _buffer_handle;
        
    };
}