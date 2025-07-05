#pragma once

#include "beluga_machine.h"
#include "beluga_interthread_buffer.h"
#include "beluga_thread.h"
#include <vector>
namespace beluga_core
{

    class dualthread : public machine
    {
        public:
            dualthread(){};
            dualthread(const dualthread &b) = default;
            //operator=
            dualthread& operator=(dualthread other);
            //Swap for operator=
            friend void swap(dualthread& first, dualthread& second); 
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

