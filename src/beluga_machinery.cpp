#include "beluga_machinery.h"

namespace beluga_core
{

    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    void swap(machinery& first, machinery& second) // nothrow
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        swap(static_cast<machine&>(first), static_cast<machine&>(second));

        swap(first._primary_rx_list, second._primary_rx_list);  
        swap(first._primary_tx_list, second._primary_tx_list);  

    }

    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    machinery& machinery::operator=(machinery other) 
    {
        swap(*this, other); 
        return *this;
    }

    bool machinery::read_config()
    {
        bool read_ok = beluga_core::machine::read_config();

        std::string rx_mail_topic_key("rx_mail_topic");
        bool mail_rx_topic_ok = _ini_ptr->get_config_value(_config_file_section, rx_mail_topic_key, &_rx_mail_topic);
        if(! mail_rx_topic_ok)
        {
            _rx_mail_topic = _config_file_section ;
        }
        std::string tx_mail_topic_key("tx_mail_topic");
        bool mail_tx_topic_ok = _ini_ptr->get_config_value(_config_file_section, tx_mail_topic_key, &_tx_mail_topic);
        if(! mail_tx_topic_ok)
        {
            _tx_mail_topic = _config_file_section ;
        }
        /*
        std::string parser_config_name_key("parser_config_section");
        bool parser_config_ok = _ini_ptr->get_config_value(_config_file_section, parser_config_name_key, &_parser_config_section);
        if(! parser_config_ok)
        {
            _parser_config_section = "globals" ;
        }
        */
        /*
        bool parser_ok = _parser.initialise(_ini_ptr, _parser_config_section);
        if(! parser_ok)
        {
            error_loop_forever("machinery error: could not initialise the string parser");
        }
        */
        return read_ok;
    }

    bool machinery::add_rx(std::list<std::string> & rx, bool overwrite_not_append )
    {
        if(overwrite_not_append)
        {
            _primary_rx_list.clear();//Overwrite
        }
        _primary_rx_list = rx;

        return true;
    }

    bool machinery::get_tx(std::list<std::string> & tx, bool overwrite_not_append)
    {
        if(overwrite_not_append)
        {
            tx.clear();//Overwrite
        }
        tx = _primary_tx_list;
        _primary_tx_list.clear();
        return true;
    }

    

}