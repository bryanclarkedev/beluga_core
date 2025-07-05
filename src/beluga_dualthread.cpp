#include "beluga_dualthread.h"
#include "beluga_factory.h"

namespace beluga_core
{

    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    dualthread& dualthread::operator=(dualthread other)
    {
        swap(*this, other); 

        return *this;
    }

    //copy-and-sep for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    void swap(dualthread& first, dualthread& second)
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        // by swapping the members of two objects,
        // the two objects are effectively swapped
        //swap<machine> calls swap<device> etc so we don't need to here
        swap(static_cast<machine &>(first), static_cast<machine &>(second));

        swap(first._app_name, second._app_name);
        swap(first._thread1, second._thread1);
        swap(first._thread2, second._thread2);
        swap(first._buffer1, second._buffer1);
        swap(first._buffer2, second._buffer2);
        swap(first._thread_names, second._thread_names);
        swap(first._buffer_names, second._buffer_names);
        swap(first._started, second._started);
    }

    /*!
    Config variables:
    app_name: string
    thread_names: the names of each thread (there will be two): string1,string2
    buffer_names: the names of each interthread buffer (there will be two): string1,string2

    e.g.:

    [dualthread1]
    app_name = aname1
    thread_names = t1,t2
    buffer_names  = one2two,two2one

    [t1]
    device_type = thread
    */
    bool dualthread::read_config()
    {
        beluga_core::machine::read_config(); //Calls the base config, which callss machine::read_config, which calls initialise_subdevices


        std::string app_name_key("app_name");
        //All config values are treated as optional.
        bool task_name_ok = _ini_ptr->get_config_value(_config_file_section, app_name_key, &_app_name );

        std::string thread_names_key("thread_names");
        std::string thread_names_val;
        bool thread_names_ok = _ini_ptr->get_config_value(_config_file_section, thread_names_key, &thread_names_val );
        if(!thread_names_ok)
        {
            beluga_utils::debug_print_loop_forever("beluga_dualthread: config error: bad thread_names");
        }

        std::string buffer_names_key("buffer_names");
        std::string buffer_names_val;
        bool buffer_names_ok = _ini_ptr->get_config_value(_config_file_section, buffer_names_key, &buffer_names_val );
        if(!buffer_names_ok)
        {
            beluga_utils::debug_print_loop_forever("beluga_dualthread: config error: bad buffer_names");
        }

        std::string delimiter = ",";
        _thread_names = beluga_utils::split_string(thread_names_val, delimiter);
        if(_thread_names.size() != 2)
        {
            beluga_utils::debug_print_loop_forever("dualthread_error: incorrect number of thread names (should be 2)");
        }else{
            Serial.print("Got thread names: ");
            Serial.print(_thread_names[0].c_str());
            Serial.print(" and ");
            Serial.println(_thread_names[1].c_str());
        }
        _buffer_names = beluga_utils::split_string(buffer_names_val, delimiter);
        if(_buffer_names.size() != 2)
        {
            beluga_utils::debug_print_loop_forever("dualthread_error: incorrect number of buffer names (should be 2)");
        }

        _thread1 = std::static_pointer_cast<beluga_core::thread>(_subdevices[_thread_names[0]]);
        _thread2 = std::static_pointer_cast<beluga_core::thread>(_subdevices[_thread_names[1]]);
        #if 1
        _buffer1 = std::static_pointer_cast<beluga_core::interthread_buffer>(_comms_map[_buffer_names[0]]);
        _buffer2 = std::static_pointer_cast<beluga_core::interthread_buffer>(_comms_map[_buffer_names[1]]);
        #endif
 
 /*
        #if 0
        _thread1 = std::make_shared<beluga_core::thread>();
        _thread2 = std::make_shared<beluga_core::thread>();
        #endif
        _thread1->initialise(_ini_ptr, _thread_names[0]); //THIS MAY BE REDUNDANT
        _thread2->initialise(_ini_ptr, _thread_names[1]);
        #if 0
        _buffer1 = std::make_shared<beluga_core::interthread_buffer>();
        _buffer2 = std::make_shared<beluga_core::interthread_buffer>();
        #endif
        _buffer1->initialise(_ini_ptr, _buffer_names[0]); //THIS MAY BE REDUNDANT
        _buffer2->initialise(_ini_ptr, _buffer_names[1]);
        */
        //The buffers are unidirectional. TX for one is RX for the other.
        _thread1->set_buffers(_buffer1, _buffer2);
        _thread2->set_buffers(_buffer2, _buffer1);


        return true;           
    }

    //Call this from loop()
    void dualthread::kill_main_thread()
    {
        //We kill this task
        vTaskDelete(NULL);
    }

    bool dualthread::run(void *)
    {
        if(! _started)
        {
            _thread1->start_thread();
            _thread2->start_thread();
        }
        return true;
    }
}

