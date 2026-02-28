#pragma once
#include "beluga_mechanism.h"
//#include "extended_factory.h"
#include "beluga_ini_reader.h"
#include "beluga_factory.h"

namespace beluga_core
{
    /*!
    \brief 
    Mechanism inherits both value_map<T> and device
    Machine contains a list of subdevices (which can be mechanisms or machines)
    This is a very flexible class and can be used for many things e.g.: IMU driver, PID controller, joystick reader

    Subdevices and comms devices are instantiated via a factory class.

    Machines can be nested in machines in a tree structure. It's possible to create one top-level machine that contains every other device in the system.
    */
    class machine : public device
    {
        public:
            machine(){};
            //Copy constructor
            machine(const machine &b) = default;
            //operator=
            machine& operator=(machine other);
            //Swap for operator=
            friend void swap(machine& first, machine& second); 

            virtual bool read_config();
            virtual bool get_factory(std::shared_ptr<beluga_core::factory> this_factory);
            virtual bool run(void * p = nullptr);

            template<typename T>
            bool set_value(std::string mechanism_name, T t, std::string value_name = "")
            {
                std::shared_ptr<beluga_core::device> this_device;
                std::shared_ptr<beluga_core::mechanism<T> > this_mechanism ;
                bool got_subdevice = get_subdevice(mechanism_name, this_mechanism);
                //this_mechanism = std::static_pointer_cast<beluga_core::mechanism<T> >(this_device);
                return this_mechanism->set_value(t, value_name);
            }

            template<typename T>
            bool set_setpoint(std::string mechanism_name, T t, std::string value_name = "")
            {
                return set_value(mechanism_name, t, value_name);
            }

            template<typename T>
            bool get_value(std::string mechanism_name, T & t, std::string value_name = "")
            {
                //std::shared_ptr<beluga_core::device> this_device;
                std::shared_ptr<beluga_core::mechanism<T> > this_mechanism ;
                bool got_subdevice = get_subdevice(mechanism_name, this_mechanism);
                //this_mechanism = std::static_pointer_cast<beluga_core::mechanism<T> >(this_device);
                return this_mechanism->get_value(t, value_name);
            }


            /*
            Under the hood, get_setpoint and get_state are the same.
            Either could be used for the other; they exist to make code cleaner.
            */
            template<typename T>
            bool get_setpoint(std::string mechanism_name, T & t, std::string value_name = "")
            {
                return get_value(mechanism_name, t, value_name);
            }

            template<typename T>
            bool get_state(std::string mechanism_name, T & t, std::string value_name = "")
            {
                return get_value(mechanism_name, t, value_name);
            }

            template<typename T>
            bool get_subdevice(std::string s,  std::shared_ptr<T> & return_val)            
            {
                for(auto iter = _subdevices.begin(); iter != _subdevices.end(); iter++)
                {
                    if(iter->first == s)
                    {
                        //return_val = _subdevices[s];
                        std::shared_ptr<beluga_core::device> this_device = _subdevices[s];
                        return_val = std::static_pointer_cast<T>(this_device);
                        return true;
                    }
                }
                return false;
            }


            

            /*
            Run an individual subdevice
            */
            template<typename T>
            //bool run(std::string mechanism_name, T t, std::string value_name = "")
            bool run(std::string device_name,  std::shared_ptr<T> t_ptr, std::string value_name = "")
            {
                std::shared_ptr<beluga_core::device> this_device;
                //std::shared_ptr<beluga_core::mechanism<T> > this_mechanism ;
                bool got_subdevice = get_subdevice(device_name, this_device);
                if(! got_subdevice)
                {
                    return false;
                }
                t_ptr = std::static_pointer_cast<T>(this_device);
                return t_ptr->run();
            }

        protected:
            std::map<std::string, std::shared_ptr<beluga_core::device> > _subdevices;
            std::vector<std::string> _subdevice_names;
            std::vector<std::string> _subdevice_types;



            //bool get_config_list_field(std::shared_ptr<beluga_utils::ini_reader> ini, std::string config_key, std::vector<std::string> & results_vec, std::string delim=",");
            virtual bool initialise_subdevices();
            virtual bool initialise_subdevice(std::string subdevice_name, std::string subdevice_type);
    };

}