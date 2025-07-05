#include "beluga_comms.h"
#include "beluga_constants.h"
namespace beluga_core
{
        
    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    comms& comms::operator=(comms other) 
    {
        swap(*this, other); 
        return *this;
    }

    //copy-and-sep for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    void swap(comms& first, comms& second) // nothrow
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        swap(static_cast<beluga_core::device&>(first), static_cast<beluga_core::device&>(second));
        swap(first._tx_topic_list, second._tx_topic_list);
        swap(first._rx_topic_list, second._rx_topic_list);
        swap(first._tx_queue, second._tx_queue);
        swap(first._rx_queue, second._rx_queue);
        swap(first._tx_time_ms, second._tx_time_ms);
        swap(first._rx_time_ms, second._rx_time_ms);
    }

    bool comms::read_config()
    {
        std::string tx_topics_list_val;
        std::string rx_topics_list_val;
        bool tx_topics_ok = _ini_ptr->get_config_value(_config_file_section, beluga_utils::config_tx_topics_list_key, &tx_topics_list_val );
        bool rx_topics_ok = _ini_ptr->get_config_value(_config_file_section, beluga_utils::config_rx_topics_list_key, &rx_topics_list_val );
    
        if((!tx_topics_ok) && (!rx_topics_ok))
        {
            //Initialise default topics as if no tx_topics AND no rx_topics listed
            //Basically, no named topic channels e.g. a serial connection
            initialise_tx_topic(beluga_utils::default_topic);
            initialise_rx_topic(beluga_utils::default_topic);
        }

        if(tx_topics_ok)
        {
            std::vector<std::string> tx_topics = beluga_utils::split_string(tx_topics_list_val, beluga_utils::topic_list_delimiter);
            for(auto iter = tx_topics.begin(); iter != tx_topics.end(); iter++)
            {
                _tx_topic_list.push_back(*iter);
            }
        }

        if(rx_topics_ok)
        {
            std::vector<std::string> rx_topics = beluga_utils::split_string(rx_topics_list_val, beluga_utils::topic_list_delimiter);
            _rx_topic_list.clear();
            for(auto iter = rx_topics.begin(); iter != rx_topics.end(); iter++)
            {
                std::stringstream ss;
                ss << "Reading config rx topic: " << iter->c_str();
                beluga_utils::debug_print(ss.str());
                _rx_topic_list.push_back(*iter);
            }
        }
        return true;
    }

    bool comms::initialise_tx_topic(std::string topic_str)
    {
        _tx_topic_list.push_back(topic_str);
        std::list<std::string> l;
        _tx_queue[topic_str] = l;
        _tx_time_ms[topic_str] = 0;
        return true;
    }

    bool comms::initialise_rx_topic(std::string topic_str)
    {
        _rx_topic_list.push_back(topic_str);
        std::list<std::string> l;
        _rx_queue[topic_str] = l;
        _rx_time_ms[topic_str] = 0;
        return true;
    }

    bool comms::check_if_tx_topic_exists(std::string topic)
    {
        bool topic_found = false;
        for(auto iter = _tx_queue.begin(); iter != _tx_queue.end(); iter++)
        {
            if(iter->first == topic)
            {
                topic_found = true;
                break;
            }
        }
        return topic_found;
    }

    bool comms::check_if_rx_topic_exists(std::string topic)
    {
        bool topic_found = false;
        for(auto iter = _rx_queue.begin(); iter != _rx_queue.end(); iter++)
        {
            if(iter->first == topic)
            {
                topic_found = true;
                break;
            }
        }
        return topic_found;
    }

    /*
    By default we check that the topic is already known to us
    This will waste CPU cycles most of the time
    It can be turned off by setting skip_topic_check to True
    */
    void comms::add_to_tx_queue(std::string s, std::string topic_str, bool add_to_front, bool force_transmit_now, bool skip_topic_check)
    {
        if(topic_str == beluga_utils::default_topic)
        {
            skip_topic_check = true;
        }
        if(! skip_topic_check)
        {
            //Check if topic list exists
            bool topic_found = check_if_tx_topic_exists(topic_str);
            if(!topic_found)
            {
                //New publication topic.
                initialise_tx_topic(topic_str);
            }
        }
        
        if(add_to_front)
        {
            _tx_queue[topic_str].push_front(s);
        }else{
            _tx_queue[topic_str].push_back(s);
        }
        if(force_transmit_now)
        {
            run_tx(topic_str);
        }
    }


    /*
    By default we check that the topic is already known to us
    This will waste CPU cycles most of the time
    It can be turned off by setting skip_topic_check to True
    */
    void comms::add_to_rx_queue(std::string s, std::string topic_str, bool add_to_front, bool skip_topic_check)
    {
        if(topic_str == beluga_utils::default_topic)
        {
            skip_topic_check = true;
        }
        if(! skip_topic_check)
        {
            //Check if topic list exists
            bool topic_found = check_if_rx_topic_exists(topic_str);
            if(!topic_found)
            {
                //New publication topic.
                initialise_rx_topic(topic_str);
            }
        }
        
        if(add_to_front)
        {
            _rx_queue[topic_str].push_front(s);
        }else{
            _rx_queue[topic_str].push_back(s);
        }
        _rx_time_ms[topic_str] = millis();
    }


    bool comms::get_rx_queue(std::list<std::string> & this_queue, std::string topic_str, bool clear_rx_queue_after, bool skip_topic_check )
    {
        bool return_val = false;

        if(! skip_topic_check)
        {
            bool rx_topic_found = check_if_rx_topic_exists(topic_str);
            if(! rx_topic_found)
            {
                return false;
            }
        }


        if(_rx_queue[topic_str].size() > 0)
        {
            //https://stackoverflow.com/questions/68381752/how-to-move-all-elements-from-a-list-to-an-array-or-a-vector-or-anything-else#:~:text=You%20use%20the%20std%3A%3A,often%20be%20a%20lot%20faster.        for(iter = _rx_queue.begin(); iter != _rx_q
            this_queue = std::list<std::string>(std::begin(_rx_queue[topic_str]), std::end(_rx_queue[topic_str]));
            return_val = true;
        }
        if(clear_rx_queue_after)
        {
            clear_rx_queue(topic_str);
        }
        return return_val;
    }

    void comms::clear_rx_queue(std::string topic_str)
    {
        _rx_queue[topic_str].clear();
    }

    //Only returns one message!
    bool comms::get_rx_msg(std::string & s, std::string topic_str, bool pop_from_queue )
    {
        
        bool return_val = false;
        try
        {
            if(_rx_queue[topic_str].size() > 0)
            {
                //https://stackoverflow.com/questions/68381752/how-to-move-all-elements-from-a-list-to-an-array-or-a-vector-or-anything-else#:~:text=You%20use%20the%20std%3A%3A,often%20be%20a%20lot%20faster.        for(iter = _rx_queue.begin(); iter != _rx_q
                s = _rx_queue[topic_str].front();
                return_val = true;
           
                if(pop_from_queue)
                {
                    _rx_queue[topic_str].pop_front();
                }
            }
        }catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            Serial.println(exc.what());
        }
        return return_val;
    }


    //Only returns one message!
    bool comms::get_tx_msg(std::string & s, std::string topic_str, bool pop_from_queue )
    {
        bool return_val = false;
        if(_tx_queue[topic_str].size() > 0)
        {
            //https://stackoverflow.com/questions/68381752/how-to-move-all-elements-from-a-list-to-an-array-or-a-vector-or-anything-else#:~:text=You%20use%20the%20std%3A%3A,often%20be%20a%20lot%20faster.        for(iter = _rx_queue.begin(); iter != _rx_q
            s = _tx_queue[topic_str].front();
            return_val = true;
        
            if(pop_from_queue)
            {
                _tx_queue[topic_str].pop_front();
            }
        }
        return return_val;
    }



    /*
    Return a a dictionary keyed by topic name
    */
    std::string comms::get_json_report()
    {
        _ss.str("");
        _ss << "{\"time_ms\": " << _time_ms;
        #if 0
        for(auto iter = _tx_topic_list.begin(); iter != _channel_list.end(); iter++)
        {
            _ss << ", \"" << *iter << "\": "
            _ss << get_json_report[*iter];
            if(*iter != _channel_list[-1])
            {
                _ss << ", ";
            }
        }
        #endif
        _ss << "}";
        return _ss.str();
    }


    std::string comms::get_json_report(std::string topic_str)
    {
        _ss.str("");
        _ss << "{\"time_ms\": " << _time_ms << ", \"_rx_time_ms\": " << _rx_time_ms[topic_str] << ", \"_tx_time_ms\": " << _tx_time_ms[topic_str] <<"}";
        return _ss.str();
    }

    bool comms::run(void * p)
    {
        _time_ms = millis();
        run_rx();
        run_tx();
        return true;
    }

    bool comms::run(std::string topic_str)
    {
        _time_ms = millis();
        run_rx(topic_str);
        run_tx(topic_str);
        return true;
    }

    bool comms::run_rx()
    {
        return false;
    }

    bool comms::run_tx()
    {
        return false;
    }

    bool comms::run_rx(std::string topic_str)
    {
        return false;
    }

    bool comms::run_tx(std::string topic_str)
    {
        return false;
    }

    bool comms::has_rx(std::string topic_str)
    {
        if(topic_str == beluga_utils::wildcard_topic)
        {
            //Return true if ANY message RXed in any topic
            for(auto rx_queue_iter = _rx_queue.begin(); rx_queue_iter != _rx_queue.end(); rx_queue_iter++)
            {
                if(rx_queue_iter->second.empty() == false)
                {
                    return true;
                }
            }
            return false;
        }
        //REturn true if message received in THIS topic
        //Check that topic is in rx_queue
        for(auto rx_queue_iter = _rx_queue.begin(); rx_queue_iter != _rx_queue.end(); rx_queue_iter++)
        {
            if(rx_queue_iter->first == topic_str)
            {
                if(rx_queue_iter->second.empty() == false)
                {
                    return true;
                }
            }
        }
        return false;
    }
}