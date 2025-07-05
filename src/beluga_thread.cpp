#include "beluga_thread.h"

/*
We populate a beluga_core::thread object
Call start_thread to get it set up
This creates the thread and starts it running 
It calls task_function 

*/

namespace beluga_core
{

    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    thread& thread::operator=(thread other)
    {
        swap(*this, other); 

        return *this;
    }

    //copy-and-sep for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    void swap(thread& first, thread& second)
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        // by swapping the members of two objects,
        // the two objects are effectively swapped
        swap(static_cast<machine &>(first), static_cast<machine &>(second));

        swap(first._task_handle, second._task_handle);
        swap(first._task_name, second._task_name);
        swap(first._rx_buffer, second._rx_buffer);
        swap(first._tx_buffer, second._tx_buffer);
        swap(first._core_id, second._core_id);
        swap(first._stack_size_bytes, second._stack_size_bytes);
        swap(first._thread_started, second._thread_started);
        //swap(first._machinery_ptr, second._machinery_ptr);
        //swap(first._machinery_name, second._machinery_name);


    }

    /*
    [thread_name1]
    serial_debug_enabled = true
    enabled = true
    task_name = tname1
    task_core = 0
    stack_size_bytes = 10000
    task_object = object_name1
    */
    bool thread::read_config()
    {
        beluga_core::machine::read_config(); //Calls the base config, which calls initialise_subdevices

        std::string task_name_key("task_name");
        std::string task_name_val;
        //All config values are treated as optional.
        bool task_name_ok = _ini_ptr->get_config_value(_config_file_section, task_name_key, &task_name_val );
        if(task_name_ok)
        {
            _task_name = task_name_val;
        }
        std::string task_core_key("task_core");
        std::string task_core_val;
        bool task_core_ok = _ini_ptr->get_config_value(_config_file_section, task_core_key, &task_core_val );
        if(task_core_ok)
        {
            _core_id = stoi(task_core_val);
        }

        std::string stack_size_key("stack_size_bytes");
        std::string stack_size_val;
        bool stack_size_ok = _ini_ptr->get_config_value(_config_file_section, stack_size_key, &stack_size_val);
        if(stack_size_ok)
        {
            _stack_size_bytes = stoi(stack_size_val);
        }
        /*
        std::string task_object_name_key("task_object");
        std::string task_object_name_val;
        bool task_object_name_ok = _ini_ptr->get_config_value(_config_file_section, task_object_name_key, &task_object_name_val);
        if(task_object_name_ok)
        {
            _task_object_name = task_object_name_val;
        }
        */
        _ss.str("");
        _ss << "Got task name " << _task_name;
        beluga_utils::debug_print(_ss.str());
        _ss.str("");
        _ss << "Got task core " << _core_id;
        beluga_utils::debug_print(_ss.str());
        _ss.str("");
        _ss << "Got stack size: " << _stack_size_bytes;
        beluga_utils::debug_print(_ss.str());
        _ss.str("");

        #if 0
        std::string machinery_name_key("machinery_name");
        bool machinery_ok =  _ini_ptr->get_config_value(_config_file_section, machinery_name_key, &_machinery_name);
        if(machinery_ok)
        {
            _ss.str("");
            _ss << "Machinery name: " << _machinery_name;
            beluga_utils::debug_print(_ss.str());
            _ss.str("");

            bool got_machinery = get_subdevice(_machinery_name, _machinery_ptr);
            if(got_machinery)
            {

            }else{
                error_loop_forever("thread error: could not find primary machinery despite one being named");
            }
        }else{
            _ss.str("");
            _ss << "No primary machinery in this thread,";
            beluga_utils::debug_print(_ss.str());
            _ss.str("");
        }
        #endif
            
        
        //These buffers are used to communicate with other threads. 
        //_rx_buffer = rx_buffer;
        //_tx_buffer = tx_buffer;

        //_core_id = 0;
        return true;           

    }

    bool thread::start_thread()
    {
        if(_thread_started)
        {
            return false;
        }
        void * task_param =  this;
        xTaskCreatePinnedToCore(
                        this->task_function,   /* Task function. */
                        _task_name.c_str(),     /* name of task. */
                        _stack_size_bytes,       /* Stack size of task */
                        task_param,        /* parameter of the task. Was NULL in previous versions of this code. */
                        1,           /* priority of the task */
                        &_task_handle,      /* Task handle to keep track of created task */
                        _core_id);          /* pin task to core 0 */       
        beluga_utils::debug_print("Task created");
        
        _thread_started = true;

        return true;
    }

    /*
    This is a static function so that it can be called by the thread
    The 'parent' thread object is passed in as an argument.
    */
    void thread::task_function(void * params)
    {
        //We know the param is a void-pointer-cast of the parent obejct (a thread)
        //Static cast https://stackoverflow.com/questions/10072004/c-typecast-cast-a-pointer-from-void-pointer-to-class-pointer
        thread * parent =  static_cast<thread *>(params); 
    
        while(1)
        {
            bool PRINT_RUN_MSG = false;
            if(PRINT_RUN_MSG)
            {
                std::stringstream ss;
                ss << "Thread " << parent->_task_name << " running at " << millis();
                Serial.println(ss.str().c_str());
            }
            /*
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
            //Do stuff before running subdevices (e.g.: do comms_rx on interthread buffer or other comms)
            parent->pre_run();
            //This runs all subdevices, with the parent thread passed as argument (so it can be cast and used). If re-implemented it can do whatever
            parent->run(params);
            //Do stuff after running subdevices (e.g. do do comms_tx on interthread buffer or other comms)
            parent->post_run();

        }
    }

    
    bool thread::pre_run(void * params)
    {
        
        if(_rx_buffer == nullptr)
        {
            _ss.str("");
            _ss << _task_name << " no rx buffer!!!";
            beluga_utils::debug_print(_ss.str());

            return false;
        }
        //Read in from the interthread buffer into its internal list of received messages
        _rx_buffer->run_rx();
        
  
        
        return true;
    }

    bool thread::post_run(void * params)
    {        
        if(_tx_buffer == nullptr)
        {
            _ss.str("");
            _ss << _task_name << " no tx buffer!!!";
            beluga_utils::debug_print(_ss.str());
            return false;
        }

        _tx_buffer->run_tx();

        return true;

    }

    /*
    Inheriting classes should replace this.
    */
    bool thread::run(void * params)
    {
        //Placeholder function - this should be replaced in inheriting classes
        std::string this_msg;
 
        //We read from the _rx_buffer's internal list of messages and print to serial
        while(_rx_buffer->get_rx_msg(this_msg))
        {
            _ss.str("");
            _ss << _task_name << " thread rx: " << this_msg;
            beluga_utils::debug_print(_ss.str());
            _ss.str("");
        }


        beluga_core::machine::run(params);
        _ss.str("");

        //We will dump some placeholder messages to the _tx_buffer.
        unsigned long time_now_ms = millis();
        if(time_now_ms % 1000 == 0)
        {
            _ss << _task_name << ": " << millis();
            _tx_buffer->add_to_tx_queue(_ss.str());
            delay(1);
        }
        //delay(1000); //Delay in ms

        return true;
    }

    bool thread::set_buffers(std::shared_ptr<beluga_core::interthread_buffer> rx_buffer, std::shared_ptr<beluga_core::interthread_buffer> tx_buffer )
    {
        _rx_buffer = rx_buffer;
        _tx_buffer = tx_buffer;
        return true;
    }
}