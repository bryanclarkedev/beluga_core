#pragma once

#include <sstream>
#include <string>
#include "Arduino.h"
#include "beluga_string.h"
#include "beluga_ini_reader.h"
#include "beluga_type_hasher.h"
#include <memory> //For shared_ptr

namespace beluga_core
{
    /*!

    This is a 'runnable' object. It includes
    - initialise
    - read_config()
    - run()
    - get_json_report()
    */
    class device
    {
        public:
            device(){};
            virtual bool initialise(std::string config_file_path, std::string config_section);
            virtual bool initialise(std::shared_ptr<beluga_utils::ini_reader> ini, std::string config_section);
            virtual bool read_config();

            /*
            We are using a void pointer as the argument to run.
            This is dangerous! But it can be useful in select cases:
            - we use it in a thread to pass in arguments to the thread function - specifically a parent object which we static_cast immediately.
            If you don't have a really good reason, don't pass anything in, just leave it as nullptr.
            You have been warned.
            */
            virtual bool run(void * p = nullptr){return _enabled;}
            virtual std::string get_json_report(){return "";}
            
            //Copy constructor
            device(const device &b) = default ;

            //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
            device & operator=(device other)
            {
                swap(*this, other); 
                return *this;
            }
        
            //Swap for operator=
            friend void swap(device & first, device & second)
            {
                // enable ADL (not necessary in our case, but good practice)
                using std::swap;
                // by swapping the members of two objects,
                // the two objects are effectively swapped
                swap(first._initialised, second._initialised);
                swap(first._enabled, second._enabled);
                swap(first._iteration, second._iteration);
        
                swap(first._serial_debug_enabled, second._serial_debug_enabled);
                swap(first._json_report, second._json_report);
                swap(first._config_file_path, second._config_file_path);
                swap(first._config_file_section, second._config_file_section);
            }

            void set_enabled(std::string s);
            void set_enabled(bool b);


        protected:
            bool _initialised = false;
            bool _serial_debug_enabled = false;
            bool _enabled = true;
            unsigned long _iteration = 0;
            std::stringstream _ss;

            std::string _json_report = "";
            std::string _config_file_path = "";
            std::string _config_file_section = "";

            //Stores the last time run() was called.
            unsigned long _time_ms = 0;
            std::shared_ptr<beluga_utils::ini_reader> _ini_ptr;
            std::string _device_name = "";
            std::string _device_type = std::string(beluga_utils::type_name(this));

    };

}