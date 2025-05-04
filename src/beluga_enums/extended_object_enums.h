#pragma once

//To use the enum defined here (extended_object_enum), add #include "extended_object_enums.h"
//and reference extended_object_enum::blah for enum constants, and extended_object_enum::EnumType for a variable
//of type extended_object_enum.
//extended_object_enum::Enum2String(e) and extended_object_enum::String2Enum(s) and other tricks can be done.

/*
//Copied from https://www.codeproject.com/Articles/32000/Improving-C-Enums-Adding-Serialization-Inheritance
The key #defines from this file are IMPROVED_ENUM_NAME, IMPROVED_ENUM_FILE,
IMPROVED_ENUM_INHERITED_NAME and IMPROVED_ENUM_INHERITED_FILE

This code is all about generating 'better' enums that are more capable, more flexible, and can be inherited from
It is a great piece of code. It works so well I haven't bothered to look at how it works.

IMPROVED_ENUM_NAME is the name of our 'better' enum, generated from the file indicated by IMPROVED_ENUM_FILE
IMPROVED_ENUM_INHERITED_NAME is the name of a 'better' enum that IMPROVED_ENUM_NAME inherits from. IMPROVED_ENUM_INHERITED_NAME is itself
generated in a file indicated by IMPROVED_ENUM_INHERITED_FILE
*/


// Declaration of the extended enum
// (extended_object_enum inherits from core_object_enum, extended with the enums listed in Generic_Sensor_Enum):
#define IMPROVED_ENUM_NAME extended_object_enum
#define IMPROVED_ENUM_FILE "extended_object_enums_src.h"
#define IMPROVED_ENUM_INHERITED_NAME  core_object_enum
#define IMPROVED_ENUM_INHERITED_FILE "core_object_enums_src.h"
#include "DefineImprovedEnum.h"
