#pragma once
#if 0
enum class Comms_Stream{
    enum_Serial,
    enum_WiFi,
    enum_MQTT,
    enum_nCommsStreams
};

enum class enum_LED_Colour{
    RED,
    BLUE,
    GREEN,
    CYAN,
    MAGENTA,
    YELLOW,
    WHITE,
    OFF
};

enum class LED_Pattern{
    enum_PEAK, //Bright middle, dim edges, pulsing
    enum_TRAIN_INWARDS, //train towards centre of light band
    enum_TRAIN_TO_FRONT, //train to front_i
    enum_TRAIN_TO_BACK,
    enum_TRAIN_CW,
    enum_TRAIN_CCW
};

enum class Message_Priority{
    enum_LOW_PRIORITY,
    enum_MEDIUM_PRIORITY,
    enum_HIGH_PRIORITY
};


enum class enum_Sensor_Type{
    Digital_Input,
    Analog_Input,
    Button,
    LSM303_Compass_Accelerometer,
    L3G_Gyro,
    Digital_IR_Proximity_Sensor,
    Error
};

enum class enum_Actuator_Type{
    Digital_Output,
    Analog_Output,
    LED,
    Error
};

/*
This enum is used in Beluga_Device_Factory for populating Beluga_Device_List
If it's ever going to be in Beluga_Device_List it needs an enum in here,
and a corresponding entry in Beluga_Device_Factory.
*/
enum class enum_Device_Type{
    enum_Zumo_Motor,
    enum_Device_Tree_Node,
    Error
};

/*
enum class Device_List_Type{
    enum_Device_List,
    enum_Beluga_Zumo_Motor,
    enum_Beluga_Joystick,
    enum_Error
};
*/



enum class enum_Setpoint_Type{
    Int16,
    Float
};

enum class enum_State_Type{
    Int16,
    Float
};
#endif

//Copied from https://www.codeproject.com/Articles/32000/Improving-C-Enums-Adding-Serialization-Inheritance
#define IMPROVED_ENUM_NAME  beluga_core_object_enum
#define IMPROVED_ENUM_FILE "core_object_enums_src.h"
#include "DefineImprovedEnum.h"

