#pragma once
#include "beluga_enums/beluga_core_object_enums.h"
#include "beluga_device.h"
#include "beluga_exceptions.h"
#include "beluga_string.h"


namespace beluga_core
{
    class factory
    {
        public:
            std::shared_ptr<device> make_new_object(beluga_core_object_enum::EnumType t);
            bool try_create(std::string this_device_type_str, std::shared_ptr<device> & new_machine);
    };

}