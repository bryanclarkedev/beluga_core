#pragma once
#include <list>
#include <map>
#include "beluga_device.h"
#include "beluga_constants.h"
namespace beluga_core
{
    /*!
    \brief This is a communications class, with queues for RX and TX
    \details 
    Communications is assumed to occur on different 'topics'. The default topic is "" but some instances
    will require multiple topics that are handled differently. These must be specified in the .ini
    There is a map of lists for RX and one for TX.
    tx_list = {tx_topic_str1: [message_to_tx1, message_to_tx2, ..]}
    rx_list = {rx_topic_str1: [message_was_rx1, message_was_rx2, ..]}
    When run() is implemented, it should do something like:
    run()
    {
        run_rx();
        //other stuff....
        run_tx();
    }
    */
    class comms : public beluga_core::device
    {
        public:
            comms(){};
            //Copy constructor
            comms(const comms &b) = default;
            //operator=
            comms& operator=(comms other);
            //Swap for operator=
            friend void swap(comms& first, comms& second); 

            //virtual bool initialise(std::string config_file_path, std::string config_section);
            virtual bool read_config();

            void initialise_topic(std::string topic_str);
            std::string get_json_report();
            std::string get_json_report(std::string topic_name);
            virtual bool run(void * p = nullptr);
            virtual bool run(std::string topic_str);
            
            virtual bool initialise_tx_topic(std::string s);
            virtual bool initialise_rx_topic(std::string s);
            
            virtual bool run_rx();
            virtual bool run_rx(std::string topic_str);
            virtual bool run_tx();
            virtual bool run_tx(std::string topic_str);

            virtual bool has_rx(std::string topic_str = beluga_utils::default_topic);

            virtual bool check_if_tx_topic_exists(std::string s);
            virtual bool check_if_rx_topic_exists(std::string s);

            virtual void add_to_tx_queue(std::string msg_str, std::string topic_str = beluga_utils::default_topic, bool add_to_front = false, bool force_transmit_now = false, bool skip_topic_check = false);
            virtual void add_to_rx_queue(std::string msg_str, std::string topic_str = beluga_utils::default_topic, bool add_to_front = false, bool skip_topic_check = false);
            
            bool get_rx_queue(std::list<std::string> & msg_vec, std::string topic_str = beluga_utils::default_topic, bool clear_rx_queue = true, bool skip_topic_check = false );
            bool get_rx_msg(std::string & s, std::string topic_str = beluga_utils::default_topic, bool pop_from_queue = true);
            void clear_rx_queue(std::string topic_str = beluga_utils::default_topic);

            bool get_tx_msg(std::string & s, std::string topic_str = beluga_utils::default_topic, bool pop_from_queue = true);

        protected:
            std::map<std::string, std::list<std::string> > _tx_queue;
            std::map<std::string, std::list<std::string> > _rx_queue;
            std::vector<std::string> _tx_topic_list;
            std::vector<std::string> _rx_topic_list;

            std::map<std::string, unsigned long int>  _rx_time_ms;
            std::map<std::string, unsigned long int> _tx_time_ms;

            //Fields for reading the config file
   
            
    };
}