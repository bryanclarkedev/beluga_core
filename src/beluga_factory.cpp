#include "beluga_factory.h"

//All core object #includes go here.
#include "beluga_digital_input.h"
#include "beluga_digital_output.h"
#include "beluga_analog_input.h"
#if 0

#include "beluga_tcp_client.h"
#include "beluga_wifi_connection.h"
#include "beluga_wifi_manager.h"
#include "beluga_mqtt_client.h"
#include "beluga_mqtt_server.h"
#include "beluga_thread.h"
#include "beluga_machinery_thread.h"
#include "beluga_dualthread.h"
#include "beluga_deepsleep.h"

#include "beluga_mqtt_client_machinery.h"
#include "beluga_led_machinery.h"
#endif

namespace beluga_core
{
    std::shared_ptr<device> factory::make_new_object(beluga_core_object_enum::EnumType t)
    {
        switch (t)
        {
            case beluga_core_object_enum::digital_input:
            {
                return std::make_shared<digital_input>();
            }
            case beluga_core_object_enum::digital_output:
            {
                return std::make_shared<digital_output>();
            }
            case beluga_core_object_enum::analog_input:
            {
                return std::make_shared<analog_input>();
            }
            #if 0
            case beluga_core_object_enum::wifi_connection:
            {
                return std::make_shared<wifi_connection>();
            }
            case beluga_core_object_enum::tcp_client:
            {
                return std::make_shared<tcp_client>();
            }
            case beluga_core_object_enum::wifi_manager:
            {
                return std::make_shared<wifi_manager>();
            }
            case beluga_core_object_enum::mqtt_client:
            {
                return std::make_shared<mqtt_client>();
            }
            #if ENABLE_MQTT_SERVER
            case beluga_core_object_enum::mqtt_server:
            {
                return std::make_shared<mqtt_server>();
            }
            #endif
            case beluga_core_object_enum::deepsleep:
            {
                return std::make_shared<deepsleep>();
            }              
            case beluga_core_object_enum::thread:
            {
                return std::make_shared<thread>();
            }
            case beluga_core_object_enum::interthread_buffer:
            {
                return std::make_shared<interthread_buffer>();
            }
            case beluga_core_object_enum::dualthread:
            {
                return std::make_shared<dualthread>();
            }            
            case beluga_core_object_enum::machinery_thread:
            {
                return std::make_shared<machinery_thread>();
            } 
            case beluga_core_object_enum::mqtt_client_machinery:
            {
                return std::make_shared<mqtt_client_machinery>();
            }
            case beluga_core_object_enum::led_machinery:
            {
                return std::make_shared<led_machinery>();
            }  
            #endif
                    
            /*case beluga_core_object_enum::LSM303_Compass_Accelerometer:
            {
                return new Beluga_LSM303_CompassAccelerometer;
            }
            case beluga_core_object_enum::L3G_Gyro:
            {
                return new Beluga_L3G_Gyro;
            }
            case beluga_core_object_enum::Digital_IR_Proximity_Sensor:
            {
                return new Beluga_Digital_IR_Proximity_Sensor;
            }*/
            default:
            {
                throw_line("Bad object type enum in beluga_factory!!!!");
                return nullptr;
            }
        }
        return nullptr;
    }
    

    /*
    this_device_type_str can be generated using beluga_utils::type_name(object_var) 
    where object_var is either a variable of type object, or a shared_ptr<object>

    The magic code in the enhanced enums includes a string2enum function that will generate an enum.
    */
    bool factory::try_create(std::string this_device_type_str, std::shared_ptr<device> & new_object)
    {
        try
        {
            new_object = make_new_object(beluga_core_object_enum::String2Enum(this_device_type_str));
            return true;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return false;
        }
        
    }

}