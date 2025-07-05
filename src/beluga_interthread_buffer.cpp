#include "beluga_interthread_buffer.h"
namespace beluga_core
    {
        
    /*!
    Config variables:
    buffer size: int16_t

    e.g:
    [interthread_buffer1]
    serial_debug_enabled = True
    enabled = True
    buffer_size = 2048
    */
    bool interthread_buffer::read_config()
    {
        //beluga_core::comms::initialise(_config_file_path, _config_file_section);

        //Initialise default topics
        initialise_tx_topic("");
        initialise_rx_topic("");

        std::string buffer_size_key("buffer_size");
        std::string buffer_size_val;
        bool buffer_size_ok = _ini_ptr->get_config_value(_config_file_section, buffer_size_key, &buffer_size_val );
        if(buffer_size_ok)
        {
            _buffer_size = stoi(buffer_size_val);
        }

        _buffer_handle = xRingbufferCreate(_buffer_size, RINGBUF_TYPE_NOSPLIT);
        if (_buffer_handle == NULL) {
            std::stringstream ss;
            _ss.str("");
            _ss << "Failed to create a ring buffer!";
            //beluga_utils::throw_exception(_ss.str());
            assert(false);
        }
        return true;
    }

    #if 0
    void interthread_buffer::transmit_to_buffer(std::string s)
    {
        const char * c_str_s = s.c_str();
        //Send an item
        UBaseType_t res =  xRingbufferSend(_buffer_handle, c_str_s, s.length() * sizeof(c_str_s), pdMS_TO_TICKS(0));
        if (res != pdTRUE) 
        {        
                std::stringstream ss; 
                int x = xRingbufferGetCurFreeSize(_buffer_handle);    
                _ss << "Failed to send item: " << s << " Available size: " << x;
                throw_exception(ss.str());
        }      
    }

    bool interthread_buffer::receive_from_buffer(std::string * s)
    {
        //Receive an item from no-split ring buffer. Timeout is zero in case ring buffer is empty
        size_t item_size;
        char *item = (char *)xRingbufferReceive(_buffer_handle, &item_size, pdMS_TO_TICKS(0));
        size_t n_chars = item_size / sizeof(item);
        //Check received item
        if(item == NULL)
        {
            //Nothing to read
            return false;
        }else{
            *s = std::string(item);
            //Return Item. See https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos_additions.html
            vRingbufferReturnItem(_buffer_handle, (void *)item);
            return true;
        }
    }
    #endif

    /*
    The interthread buffer is a single queue, so we use the default topic ""
    */
    bool interthread_buffer::run_tx()
    {
        if(_tx_queue[""].size() == 0)
        {
            return false;
        }

        while(_tx_queue[""].size() > 0)
        {
            std::string s = _tx_queue[""].front();
            const char * c_str_s = s.c_str();
            //Send an item
            UBaseType_t res =  xRingbufferSend(_buffer_handle, c_str_s, s.length() * sizeof(c_str_s), pdMS_TO_TICKS(0));
            if (res != pdTRUE) 
            {        
                    std::stringstream ss; 
                    int x = xRingbufferGetCurFreeSize(_buffer_handle);    
                    _ss << "Failed to send item: " << s << " Available size: " << x;
                    //beluga_utils::throw_exception(ss.str());
                    return false;
            }else{
                _tx_time_ms[""] = millis();
                _time_ms = _tx_time_ms[""];
                //Delete the item we sent
                _tx_queue[""].pop_front();
            }
        }    
        return true;  
    }


    bool interthread_buffer::run_rx()
    {
        bool return_val = false;
        std::string rx_str;
        while(1)
        {
            //Receive an item from no-split ring buffer. Timeout is zero in case ring buffer is empty
            size_t item_size;
            char *item = (char *)xRingbufferReceive(_buffer_handle, &item_size, pdMS_TO_TICKS(0));
            size_t n_chars = item_size / sizeof(item);
            //Check received item
            if(item == NULL)
            {
                //Nothing to read
                break;
            }else{
                //_rx_time_ms[""] = millis();
                //Cast to a str
                rx_str = std::string(item);
                //Store the str
                add_to_rx_queue(rx_str);
                _time_ms = _rx_time_ms[""];
                
                //_rx_queue[""].push_back(rx_str);
                //Return Item. See https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos_additions.html
                vRingbufferReturnItem(_buffer_handle, (void *)item);
                return_val = true;
            }
        }
        return return_val;
    }


    /*
        bool create_new_processes(TaskHandle_t * core0_task_handle, TaskFunction_t core0_task_function, 
                                TaskHandle_t core1_task_handle, TaskFunction_t core1_task_function);
    */
    }