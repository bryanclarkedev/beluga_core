#pragma once
#include "beluga_machine.h"
#include "beluga_comms.h"
//#include "extended_factory.h"
#include "beluga_ini_reader.h"
//#include "beluga_message_string_processor.h"
#include "beluga_parsers.h"
/*
Device: has read_config(), run()
Value map: has map of values
comms: device with additional comms functions (run_tx, run_rx, add_to_tx_queue, add_to_rx_queue)

Mechanism: device AND value map
Machine: device with list of devices AND list of comms
Machinery: machine with a public interface for rx and tx
- add_rx(&std::list<std::string>) writes to _primary_rx_list
- get_tx(&std::list<std::string>) retrieves _primary_tx_list
These are used to handle over comms in/out of the machinery as a whole. 

Generally, use a machinery with many devices nested within, and have the machinery act as the communications interface
A use case is if there is one machinery in a thread, and it receives messages from another thread, then sends messages back. 
In between it might send messages via bluetooth comms, or read sensors or drive actuators as part of various subdevices/comms. 

*/
namespace beluga_core
{
    class machinery : public machine
    {
        public:
            machinery(){};
            //Copy constructor
            machinery(const machinery &b) = default;
            //operator=
            machinery& operator=(machinery other);
            //Swap for operator=
            friend void swap(machinery& first, machinery& second); 

            virtual bool read_config();

            virtual bool add_rx(std::list<std::string> & rx, bool overwrite_not_append = true);

            virtual bool get_tx(std::list<std::string> & tx, bool overwrite_not_append = true);

            virtual bool handle_primary_rx(){return false;}//std::std::vector< std::map<std::string, std::string> > & rx_subtopic_map_vec);//{ return false; }

            virtual bool generate_primary_tx(){return false;}

        protected:
            
            std::list<std::string> _primary_rx_list;
            std::list<std::string> _primary_tx_list;
            std::string _rx_mail_topic = "";
            std::string _tx_mail_topic = "";
            //message_string_processor _parser;
            std::string _parser_config_section = "";



    };

}