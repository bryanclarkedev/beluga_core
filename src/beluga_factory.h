#pragma once
#include "beluga_enums/beluga_core_object_enums.h"
#include "beluga_device.h"
#include "beluga_exceptions.h"
#include "beluga_string.h"

/*
We use a factory class to create many different types of objects that all inherit from beluga_core::device.

TODO: Make it easier to add in new device types. Right now it involves:
- write new_device_class
- add new_device_class to beluga_factory.cpp
- add new device to core_object_enums_src.h
*/
namespace beluga_core
{
    class factory
    {
        public:
            std::shared_ptr<device> make_new_object(beluga_core_object_enum::EnumType t);
            bool try_create(std::string this_device_type_str, std::shared_ptr<device> & new_machine);
    };

}