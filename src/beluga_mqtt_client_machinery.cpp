#include "beluga_mqtt_client_machinery.h"
#include "beluga_parsers.h"
#include <map>
namespace beluga_core
{
/*
    bool mqtt_client_machinery::read_config()
    {
        bool config1_ok = beluga_core::machinery::read_config();
        

        bool mqtt_client_in_subdevices = false;

        auto iter1 = _comms_names.begin();
        auto iter2 = _comms_types.begin();
        for( ; iter1 != _comms_names.end() && iter2 != _comms_types.end(); ++iter1, ++iter2)
        {
             Serial.print("Got comms device: ");
            Serial.print(iter1->c_str());
             Serial.print(" Got type: ");
            Serial.println(iter2->c_str());
            if((*iter2) == "mqtt_client")
            {
                mqtt_client_in_subdevices = true;
                _mqtt_client_name = *iter1;
                Serial.println("!!!!!!!");
                break;
            }
        }

        if(! mqtt_client_in_subdevices)
        {
            beluga_utils::debug_print_loop_forever("mqtt_client_machinery error: no mqtt client in comms list!");
        }

        //_mqtt_ptr = std::make_shared<beluga_core::mqtt_client>(); //This line may be redundant
        bool mqtt_ok = get_comms(_mqtt_client_name, _mqtt_ptr);
        if(! mqtt_ok)
        {
            beluga_utils::debug_print_loop_forever("mqtt_client_machinery error: could not get mqtt client");
        }

        std::string rx_topics_key("rx_topics");
        std::string rx_topics_str;
        bool got_rx_topics_str = _ini_ptr->get_config_value(_mqtt_client_name, rx_topics_key, &rx_topics_str);
        if(! got_rx_topics_str)
        {
            //No rx topics in mqtt - tx only. This is allowed.
        }else{
            _mqtt_rx_topics = beluga_utils::split_string(rx_topics_str, ",");
        }

        return config1_ok;
    }
*/
    /*
    Primary rx is passed into the mqtt_client_machinery from the interthread buffer in the thread.beluga_core
    Primary rx is in the form of <this_device_name><delim><topic name><delim><payload>
    */
   /*
    bool mqtt_client_machinery::handle_primary_rx() //std::vector< std::map<std::string, std::string> > & rx_subtopic_map_vec)
    {
        for(auto rx_iter = _primary_rx_list.begin(); rx_iter != _primary_rx_list.end(); rx_iter++)
        {
            std::string this_str = *rx_iter;
            
            std::vector<std::string> this_split_string = beluga_utils::from_interthread_buffer(this_str);
            if(this_split_string[0] != _device_name)
            {
                continue; //Something is wrong!
            }
            std::string this_topic = this_split_string[1];
            std::string this_payload = this_split_string[2];
            _mqtt_ptr->add_to_tx_queue(this_payload, this_topic);

            #if 0
            std::string this_topic;

            bool got_topic_ok = _parser.get_topic(this_str, this_topic);
            if(got_topic_ok)
            {
                std::string this_payload;
                bool got_payload_ok = _parser.get_payload(this_str, this_payload );
                if(got_payload_ok)
                {
                    _mqtt_ptr->add_to_tx_queue(this_payload, this_topic);
                }
            }
            #endif
        }
        #define NEWMETHOD 0
        #if NEWMETHOD
        for(auto rx_iter = _primary_rx_list.begin(); rx_iter != _primary_rx_list.end(); rx_iter++)
        {
            std::map<std::string, std::string> this_map;
            beluga_utils::deserialise(*rx_iter,this_map ); //We have a string of key1=val1,key2=val2. Return it as a map {key1: val1, key2:val2}
            //_parser.deserialise(*rx_iter, this_map);
            rx_subtopic_map_vec.push_back(this_map);
            for(auto map_iter = this_map.begin(); map_iter != this_map.end(); map_iter++)
            {
                std::string subtopic = map_iter->first;
                _ss.str("");
                std::string topic;
                bool got_topic  = _parser.get_topic(*rx_iter, topic);
                if(got_topic)
                {
                    _ss << topic << "/" << subtopic; //append the subtopic to the topic
                    topic = _ss.str();
                }else{
                    Serial.println("Could not get topic");
                    continue;
                }
                std::string message = map_iter->second;
                _mqtt_ptr->add_to_tx_queue(message, topic);

            }
        }
        


        #else
        

           // std::map<std::string, std::string> this_map;
           // _parser.deserialise(*rx_iter, this_map);
            //rx_subtopic_map_vec.push_back(this_map);
     
        #endif
       #if 0
            std::string topic_delim = _topic_delimiter;
            std::vector<std::string> publish_vec;
            for(auto rx_iter = _primary_rx_list.begin(); rx_iter != _primary_rx_list.end(); rx_iter++)
            {
                //_mqtt_ptr->add_to_tx_queue(*rx_iter, "test1/val1"); //This will publish on the mqtt default tx topic (beluga/mqtt by default)

                publish_vec = beluga_utils::split_string(*rx_iter, topic_delim);
                if(publish_vec.size() != 2)
                {
                    //Serial.println("Cannot publish rx str to mqtt topic; sending to fail topic");
                    _mqtt_ptr->add_to_tx_queue(*rx_iter); //, "test1/val1"); //This will publish on the mqtt default tx topic (beluga/mqtt by default)
                }else{
                    Serial.println("DEBUG 123");
                    _mqtt_ptr->add_to_tx_queue(publish_vec[1], publish_vec[0]);
                }

            }
        #endif
        _primary_rx_list.clear();

        return false;

    }
*/
    /*
    Rx from mqtt broker -> tx to primary comms
    to_interthread_buffer generates <device name><delim><topic><delim><payload>
    We assume the recipient machinery knows how to decode it.
    */
   /*
    bool mqtt_client_machinery::generate_primary_tx()
    {
        
        for(auto mqtt_rx_topic_iter = _mqtt_rx_topics.begin(); mqtt_rx_topic_iter != _mqtt_rx_topics.end(); mqtt_rx_topic_iter++)
        {
            std::list<std::string> mqtt_rx_vec;
            bool got_mqtt_ok = _mqtt_ptr->get_rx_queue(mqtt_rx_vec, *mqtt_rx_topic_iter);
            if(got_mqtt_ok)
            {
                // Create strings of format: <mqtt topic><topic delimiter><mqtt payload>
                for(auto rx_payload_iter = mqtt_rx_vec.begin(); rx_payload_iter != mqtt_rx_vec.end(); rx_payload_iter++)
                {
                    //Add to the tx interthread buffer
                    #if 0
                    std::string prepended_str;
                    bool prepended_str_ok = _parser.prepend_topic_to_payload(*mqtt_rx_topic_iter, *rx_payload_iter, prepended_str);
                    if(! prepended_str_ok)
                    {
                        continue;
                    }
                    _primary_tx_list.push_back(prepended_str);
                    Serial.print("Forwarding mqtt rx: ");
                    Serial.println(prepended_str.c_str());
                    #endif
                }
            }
        }
        return true;
    }

    bool mqtt_client_machinery::run(void * p)
    {
        std::vector< std::map<std::string, std::string> > rx_subtopic_map_vec;
        bool primary_rx_ok = handle_primary_rx();//(rx_subtopic_map_vec);
         

        beluga_core::machinery::run();

        bool primary_tx_ok = generate_primary_tx();
        return true;
    }
*/

}