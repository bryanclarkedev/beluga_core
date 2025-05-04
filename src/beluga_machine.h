#pragma once
#include "beluga_mechanism.h"
//#include "extended_factory.h"
#include "beluga_ini_reader.h"
#include "beluga_comms.h"
#include "beluga_factory.h"

namespace beluga_core
{
    /*!
    \brief 
    Mechanism inherits both value_map<T> and device
    Comms inherits device
    Machine is a device containing a list of devices (which can be mechanisms or machines) AND a list of comms
    Sometimes only the devices will be used, sometimes only the comms will be used, sometimes both will be used
    This is a very flexible class and can be used for many things e.g.: IMU, PID controller, joystick reader

    Due to how machines can be nested in machines
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
            virtual bool read_config_comms();
            //Sometimes we use pre-existing comms
            virtual bool set_comms(std::map<std::string, std::shared_ptr<beluga_core::comms> > & comms_map, std::vector<std::string> comms_types);
            virtual bool get_factory(std::shared_ptr<factory> this_factory);
            virtual bool run(void * p = nullptr);

            template<typename T>
            bool set_setpoint(std::string mechanism_name, T t, std::string value_name = "")
            {
                std::shared_ptr<beluga_core::device> this_device;
                std::shared_ptr<beluga_core::mechanism<T> > this_mechanism ;
                bool got_subdevice = get_subdevice(mechanism_name, this_device);
                this_mechanism = std::static_pointer_cast<beluga_core::mechanism<T> >(this_device);
                return this_mechanism->set_value(t, value_name);
            }

            template<typename T>
            bool get_setpoint(std::string mechanism_name, T & t, std::string value_name = "")
            {
                std::shared_ptr<beluga_core::device> this_device;
                std::shared_ptr<beluga_core::mechanism<T> > this_mechanism ;
                bool got_subdevice = get_subdevice(mechanism_name, this_device);
                this_mechanism = std::static_pointer_cast<beluga_core::mechanism<T> >(this_device);
                return this_mechanism->get_value(t, value_name);
            }

            template<typename T>
            bool get_state(std::string mechanism_name, T & t, std::string value_name = "")
            {
                std::shared_ptr<beluga_core::device> this_device;
                std::shared_ptr<beluga_core::mechanism<T> > this_mechanism;
                bool got_subdevice = get_subdevice(mechanism_name, this_device);
                this_mechanism = std::static_pointer_cast<beluga_core::mechanism<T> >(this_device);
                return this_mechanism->get_value(t, value_name);
            }

            bool get_subdevice(std::string s,  std::shared_ptr<beluga_core::device> & return_val)            
            {
                for(auto iter = _subdevices.begin(); iter != _subdevices.end(); iter++)
                {
                    if(iter->first == s)
                    {
                        return_val = _subdevices[s];
                        //std::shared_ptr<beluga_core::device> this_device = _subdevices[s];
                        //return_val = std::static_pointer_cast<T>(this_device);
                        return true;
                    }
                }
                return false;
            }


            template<typename T>
            bool get_comms(std::string s, std::shared_ptr<T> & return_val)
            {
                for(auto iter = _comms_map.begin(); iter != _comms_map.end(); iter++)
                {
                    if(iter->first == s)
                    {
                        return_val = _comms_map[s];
                        //std::shared_ptr<beluga_core::comms> this_comms = _comms_map[s];
                        //return_val = std::static_pointer_cast<T>(this_comms);
                        return true;
                    }
                }
                return false;
            }

            template<typename T>
            //bool run(std::string mechanism_name, T t, std::string value_name = "")
            bool run(std::string mechanism_name,  std::shared_ptr<T> t_ptr, std::string value_name = "")
            {
                std::shared_ptr<beluga_core::device> this_device;
                //std::shared_ptr<beluga_core::mechanism<T> > this_mechanism ;
                bool got_subdevice = get_subdevice(mechanism_name, this_device);
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

            std::map<std::string, std::shared_ptr<beluga_core::comms> > _comms_map;
            std::vector<std::string> _comms_names;
            std::vector<std::string> _comms_types;

            //bool get_config_list_field(std::shared_ptr<beluga_utils::ini_reader> ini, std::string config_key, std::vector<std::string> & results_vec, std::string delim=",");
            virtual bool initialise_subdevices();
            virtual bool initialise_subdevice(std::string subdevice_name, std::string subdevice_type);
    };

}