#pragma once

#include "beluga_machine.h"
#include "beluga_interthread_buffer.h"
#include "beluga_machinery.h"

/*
A beluga_core::thread is a special type of machine
It implements a FreeRTOS task pinned to a core
The thread object has a task_function function which is called by the FreeRTOS class
By default, the object's pre-run/run/post-run functions (which include comms stuff) are called in task_fucntion in the order:
- pre_run()
-- run_rx()
- run()
- post_run()
-- run_tx()

There is a 'task object' which is the singular object for the thread

Interprocess buffers are not included! They need to be assigned using set_buffers()

The thread can be made to do useful things in a couple of ways
- inherit beluga_core::thread, re-implement pre_run and post_run to handle comms, setpoints, states etc
-- No need to re-implement run(), just leave it to run all subdevices with the pre_run and post_run functions
    to handle the rest
- inherit beluga_core::thread, re-implement run()
-- pre_run and post_run are redundant here
- have just one subdevice which itself contains all the other subdevices and has a custom run(). 
-- It gets passed the void * params which points to the parent thread
-- so this object can static_cast the pointer and then access the interthread buffers and manage comms etc
*/
namespace beluga_core
{
    class thread : public machinery
    {
        public:
            thread(){};
            thread(const thread &b) = default;
            //operator=
            thread& operator=(thread other);
            //Swap for operator=
            friend void swap(thread& first, thread& second); 

            //Make these public because we have to cast the object to a void*, cast it back, and pull
            //fields out, in task_function, which is a static function
            //https://forum.arduino.cc/t/esp32-xtaskcreatepinnedtocore-usage-in-a-class/630469/2
            TaskHandle_t _task_handle;
            virtual bool read_config();//std::string config_file_path, std::string config_section, beluga_core::interthread_buffer * rx_buffer = nullptr, Beluga_Interthread_Buffer * tx_buffer = nullptr);
            bool set_buffers(std::shared_ptr<beluga_core::interthread_buffer> rx_buffer, std::shared_ptr<beluga_core::interthread_buffer> tx_buffer );

            std::string _task_name = "beluga_thread";
            std::shared_ptr<beluga_core::interthread_buffer> _rx_buffer = nullptr;
            std::shared_ptr<beluga_core::interthread_buffer> _tx_buffer = nullptr;
            virtual bool pre_run(void * params = nullptr);//{ return true; } //Inheriting class should do comms_rx etc in here.
            virtual bool post_run(void * params = nullptr);//{ return true; } //Inheriting class should do comms_tx etc in here.
            virtual bool run(void * paramas = nullptr);
            virtual bool start_thread();

        protected:
            //std::shared_ptr<beluga_core::machinery> _machinery_ptr = nullptr;
            //std::string _machinery_name = "";

            //task_function is static so that it can be called from the thread
            static void task_function(void * params);
            //std::string _task_object_name;
            //std::shared_ptr<machinery> task_object; 
            uint8_t _core_id = 0; //0 or 1 for dual-core ESP32.
            uint16_t _stack_size_bytes = 30000;
            bool _thread_started = false;

    };

}