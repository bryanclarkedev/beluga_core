#pragma once

#include "beluga_device.h"
#include "beluga_value_map.h"




namespace beluga_core
{
    /*!
    \brief mechanism<T> is a class combining data storage from value_map and useful
    'runnability' from device.
    \details     beluga_value and beluga_value_map are just data structures for storing one or 
    several data points.     By inheriting from both value_map and device we create a useful object 
    that is also a data store.
    Many simple objects can be implemented as mechanisms e.g. digital inputs

    Nearly all of our objects will inherit from mechanism.

    // Many function pointer will be stored as this type
    //Per https://stackoverflow.com/questions/2136998/using-a-stl-map-of-function-pointers
    //and https://stackoverflow.com/questions/1924844/stdmap-of-member-function-pointers
    //typedef bool (device::*machineFuncPtr)(std::string); 
    */
    template <typename T>
    class mechanism : public device, public value_map<T>
    {

            public:
                mechanism<T>(){};
                
                //Copy constructor
                mechanism<T>(const mechanism<T> &b) = default ;
                //operator=
                //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
                mechanism<T> & operator=(mechanism<T> other)
                {
                    swap(*this, other); 
                    return *this;
                }

                //Swap for operator=
                friend void swap(mechanism<T> & first, mechanism<T> & second)
                {
                    // enable ADL (not necessary in our case, but good practice)
                    using std::swap;

                    swap(static_cast<device &>(first), static_cast<device &>(second));
                    swap(static_cast<value_map<T> &>(first), static_cast<value_map<T> &>(second));

                    // by swapping the members of two objects,
                    // the two objects are effectively swapped
                }

                
    };



}