#pragma once
#include "beluga_mechanism.h"
//#include "extended_factory.h"
#include "beluga_ini_reader.h"
#include "beluga_factory.h"
#include "beluga_comms.h"
#include "beluga_machine.h"
#include "beluga_debug.h"
#include "beluga_thread.h"
#include "beluga_constants.h" //For concatenation_delimiter
#include "beluga_nexus.h"
#include "beluga_digital_output.h"
#include "beluga_temperature_pressure_bmp280.h"
#include "beluga_li_batt_fuel_gauge_max17043.h"
#include "beluga_deepsleep.h"
namespace beluga_core
{
    /*!
    \brief 
    Nexus is an example of a machine intended to be a top-level class for bigger applications. 
    You have two threads, a nexus in each thread,
    and the nexus contains all the machines/devices/etc to run everything.

    Nexus reads from the rx_buffer and appends to the tx_buffer as part of its run() function.

    */
    class solar_nexus : public nexus
    {
        private:
            std::shared_ptr<beluga_core::digital_output> status_led_ptr;
            std::shared_ptr<beluga_core::temperature_pressure_bmp280> bmp280_ptr;
            std::shared_ptr<beluga_core::li_batt_fuel_gauge> fuel_gauge_ptr;
            std::shared_ptr<beluga_core::deepsleep> deepsleep_ptr;

        public:
            solar_nexus(){};

            virtual bool read_config(){
                std::string config_val;
                
                bool config_ok = false;
                config_ok = nexus::read_config();
                if(! config_ok)
                {
                    _ss.str("");
                    _ss << "Could not read nexus config for device " << _device_name;
                    beluga_utils::debug_print_loop_forever(_ss.str());
                }
                //Solar nexus consists of
                //- a BMP280 temperature sensor
                //- a li batt fuel gauge
                //- a LED
                //- a deepsleep
                status_led_ptr = std::make_shared<beluga_core::digital_output>();
                bool got_led = get_subdevice("status_led", status_led_ptr );
                assert(got_led);
                bmp280_ptr = std::make_shared<beluga_core::temperature_pressure_bmp280>();
                bool got_bmp280 = get_subdevice("bmp280", bmp280_ptr);
                assert(got_bmp280);
                deepsleep_ptr = std::make_shared<beluga_core::deepsleep>();
                bool got_deepsleep = get_subdevice("deepsleep",deepsleep_ptr);
                assert(got_deepsleep);

                return true;
            }
            //
            virtual bool run(void * p = nullptr){
                //void pointer casting like this can be a bit dicey 
                //but we know that the pointer is to the parent thread
                beluga_core::thread * parent_thread =  static_cast<beluga_core::thread *>(p); 
                std::shared_ptr<beluga_core::interthread_buffer> rx_buffer = parent_thread->_rx_buffer;
                std::shared_ptr<beluga_core::interthread_buffer> tx_buffer = parent_thread->_tx_buffer;
                bool buffers_ok = (rx_buffer != nullptr) && (tx_buffer != nullptr);
                if(! buffers_ok){
                    beluga_utils::debug_print_loop_forever("Error in nexus getting threads");
                }else{
                    //We read from the _rx_buffer's internal list of messages and print to serial
                    std::string this_msg;
                    while(rx_buffer->get_rx_msg(this_msg))
                    {
                        this->msg_thread_buffer_to_nexus(this_msg);
                    }
                    //Run base class (which runs subdevices)
                    beluga_core::machine::run(p);
                    generate_report_string();

                    //We will dump some placeholder messages to the _tx_buffer.
                    unsigned long time_now_ms = millis();
                    unsigned long dt_ms = time_now_ms - _time_ms;
                    if(dt_ms > _comms_period_ms )
                    {
                        _time_ms = time_now_ms;
                        std::list<std::string> msg_list;
                        bool got_msg_out = msg_nexus_to_thread_buffer(msg_list);                        
                        if(got_msg_out)
                        {
                            for(auto iter = msg_list.begin(); iter != msg_list.end(); iter++)
                            {
                                //_ss.str("");
                                //_ss << _device_name << ": " << millis() << "!";
                                tx_buffer->add_to_tx_queue(*iter);
                                //_ss.str("");
                                //Serial.println(_report_string.c_str());
                            }
        
                        }
                    }
                }
                return true;
            }



            virtual bool split_and_process_msg(std::string in_msg){
                std::vector<std::string> split_str = beluga_utils::split_string(in_msg, beluga_utils::concatenation_delimiter);
                int n_fields_required = 3;
                if(split_str.size() != n_fields_required){
                    _ss.str("");
                    _ss << "Beluga nexus split msg error: Needed " << n_fields_required << " fields, got " << split_str.size();
                    beluga_utils::debug_print(_ss.str());
                    _ss.str("");
                    return false;
                }
                std::string subdevice_name = split_str[0]; //Is "" if the device is the nexus itself
                std::string state_name = split_str[1];
                std::string setpoint_value = split_str[2];
                _ss.str("");
                _ss << "Nexus got msg: Subdevice name: " << subdevice_name;
                _ss << ", State name: " << state_name;
                _ss << ", setpoint value: " << setpoint_value;
                beluga_utils::debug_print(_ss.str());
                _ss.str("");
        
                if(beluga_utils::string_is_number(setpoint_value)){
                    int i = beluga_utils::string_to_int(setpoint_value);
                    bool set_ok = this->set_setpoint(subdevice_name, i);
                    _ss << "Set ok: " << set_ok << "!!!!!!!!!!!!!!!!!!!!!!!!!!";
                    beluga_utils::debug_print(_ss.str());
                    _ss.str("");
                }else{
                    _ss.str("");
                    _ss << "Not a int: " << setpoint_value;
                                        beluga_utils::debug_print(_ss.str());
                    _ss.str("");
                }
                return true;
            }

            //Reimplement as needed for inheriting classes
            virtual bool nexus_handle_msg(std::string in_msg){
                _ss.str("");
                _ss  << "Nexus handle msg: " << in_msg;
                beluga_utils::debug_print(_ss.str());
                _ss.str("");
                return true;
            }

            /*
            Reimplement this as needed
            Typically the nexus will have its own message handling functions
            But if we don't want to implement those, 
            we can use the machine's set_setpoint. But this requires splitting up the input string
            so that we know subdevice (if there is one), state we are setting the setpoint of,
            and the setpoint value and value type.
            We could use JSON but I am not a fan of the Arduino JSON libraries
            (too much predefining message sizes).
            Make a couple of assumptions:
            - some messages will be handled by the nexus bespoke code, some may just use set_setpint.
            -- to distinguish: 
            ---if a message starts with a special character, we split it and process here
            --- otherwise pass to the nexus to handle (implementation specific!)
            - format is:
            <delim><machine name><delim><state name><delim><setpoint value>
            -- machine_name == "" if the device is the machine itself
            -- state_name can also be "". So a valid message can look like <delim><delim><delim><setpoint value>
            - in_msg will have a bunch of substrings separated by a delimter character
            (I could use  JSON or XML but dont want to muck around with the parsing)
            - if(string_is_number ) -> int
            - else if(string_is_float) -> float
            - else -> string
            - For bool: handle it as a float.
            */
            virtual bool msg_thread_buffer_to_nexus(std::string in_msg){
                _ss.str("");
                _ss << _device_name  << " Nexus got msg in: '" << in_msg << "'";
                beluga_utils::debug_print(_ss.str());
                _ss.str("");

                if(in_msg.size() == 0){
                    return false;
                }
                try
                {
                    _ss.str("");
                    _ss << in_msg[0];
                    std::string first_char_str = _ss.str();
                    _ss.str("");
                    bool parse_here = (first_char_str == beluga_utils::concatenation_delimiter) && (in_msg.size() > 1);
                    if(parse_here){
                        //Drop the first character
                        in_msg.erase(0,1); 
                       return split_and_process_msg(in_msg);
                    }else{
                        return nexus_handle_msg(in_msg);
                    }
                }
                catch(const std::exception& e)
                {
                    _ss.str("");
                    _ss << e.what() << '\n';
                    beluga_utils::debug_print(_ss.str());
                    _ss.str("");
                }
                
                return true;
            }


            
            //Reimplement this as needed
            virtual bool msg_nexus_to_thread_buffer(std::list<std::string> & out_msg_list){
                out_msg_list.clear();
                if(_report_string != "" ){
                    out_msg_list.push_back(_report_string);
                } 
                return out_msg_list.size() > 0;
            }


        protected:
            int _comms_period_ms = 1000;

    };

}