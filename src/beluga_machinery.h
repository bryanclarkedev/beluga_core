#pragma once

#include "beluga_machine.h"
#include "beluga_interthread_buffer.h"
#include "beluga_thread.h"
#include <vector>

/*
Device: has read_config(), run()
Value map: has map of values
comms: device with additional comms functions (run_tx, run_rx, add_to_tx_queue, add_to_rx_queue)

Mechanism: device AND value map
Machine: device with list of devices (including comms devices and machines nested)
Machinery: two threads, two interthread buffers. This will be the top-level class for many applications. 

*/

namespace beluga_core
{

    class machinery : public machine
    {
        public:
            machinery(){};
            machinery(const machinery &b) = default;
            //operator=
            machinery& operator=(machinery other);
            //Swap for operator=
            friend void swap(machinery& first, machinery& second); 
            bool start_threads();
            bool read_config();//std::string config_file_path, std::string config_section, beluga_core::interthread_buffer * rx_buffer = nullptr, Beluga_Interthread_Buffer * tx_buffer = nullptr);
            virtual bool run(void * params = nullptr);
            void kill_main_thread();
            std::string _app_name = "beluga_app";
        protected:
            std::shared_ptr<beluga_core::thread> _thread1 = nullptr;
            std::shared_ptr<beluga_core::thread> _thread2 = nullptr;
            std::shared_ptr<beluga_core::interthread_buffer> _buffer1 = nullptr;
            std::shared_ptr<beluga_core::interthread_buffer> _buffer2 = nullptr;

            std::vector<std::string> _thread_names;
            std::vector<std::string> _buffer_names;
            bool _started = false;
    };

}

