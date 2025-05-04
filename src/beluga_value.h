#pragma once
#include <stdint.h> //For int16_t
#include <algorithm> //For std::swap
#include "Arduino.h" //For millis()


//Per https://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor we include
//all source in the header.

//using namespace beluga_utils;
namespace beluga_core
{
    /*!
    \brief Value template class
    \author Bryan Clarke
    \date 
    \details This is a generic 'data bucket' class. I find myself having to make frequently making three or four variables 
    for every one value to track:
    - the value
    - the time the value was updated
    - whether the value changed with the last update
    - whether the value has been initialised (which you COULD get by mucking with the timestamp e.g set to 0 or whatever)
    Assuming we will never run out of memory so a bit of overkill is fine, the beluga_core::value class wraps up a bunch
    of useful flag and status information along with the value itself.

    These will be used in beluga_core::value_map; we will seldom use a naked beluga_core::value
    */
    template <typename T>
    class value
    {
        public:
            value<T>(){};

            //Copy constructor
            value<T>(const value<T> &b) = default;

            //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
            value<T> & operator=(value<T> other) 
            {
                swap(*this, other); 

                return *this;
            }

            //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
            //If we ever add any fields to value, they need to also be added here
            friend void swap(value<T> & first, value<T> & second) // nothrow
            {
                // enable ADL (not necessary in our case, but good practice)
                using std::swap;
                
                // by swapping the members of two objects,
                // the two objects are effectively swapped
                swap(first._value, second._value);
                swap(first._value_changed, second._value_changed);
                swap(first._value_changed_time_ms, second._value_changed_time_ms);
                swap(first._value_refreshed, second._value_refreshed);
                swap(first._value_refreshed_time_ms, second._value_refreshed_time_ms);
                swap(first._uninitialised, second._uninitialised);
            }

            /*!
            \brief Set the value of this beluga_core::value
            \arg T value is the new value
            \details This will automatically update the _value_refreshed and _value_refreshed_time_ms fields
            If the value has changed, it will also update _value_changed and _value_changed_time_ms
            If this is the first time a value has been set, it will set _uninitialised to false
            */
            void set_value(T value)
            {
                unsigned long time_now_ms = millis();
                _value_refreshed = true;
                _value_refreshed_time_ms = time_now_ms;

                if(_uninitialised)
                {
                    _value_changed = true;
                    _uninitialised = false;
                }else{
                    _value_changed = _value != value;
                }
                if(_value_changed)
                {
                    _value_changed_time_ms = time_now_ms;
                }
                _value = value;

                return;
            }

            /*!
            \brief Attempt to retrieve the value of this beluga_core::value
            \arg value is a passed-by-reference value that will be set to the value of this beluga_core::value if possible.
            \details If the value is uninitialised, returns False
            If initialised, sets the value of arg value and returns True.
            */
            bool get_value(T & value)
            {
                if(_uninitialised)
                {
                    return false; //Value has not been set
                }
                value = _value;

                return true;
            }
            
            /*!
            \brief Gets whether the value has been changed since the last time it was checked
            \arg boolean clear_on_read is whether the _value_changed flag should be set to False once 
            value_changed is set to TRUE when set_value() is called with a new, different value
            consecutive set_value() calls with the same value will NOT set it to true.
            calling get_value_changed() clears the flag by default.
            \return boolean value of _value_changed
            */
            bool get_value_changed(bool clear_on_read = true)
            {
                if(! clear_on_read)
                {
                    return _value_changed;
                }
                bool value_changed_temp = get_value_changed(false);
                clear_value_changed();
                return value_changed_temp;
            }

            /*!
            \brief Clears the _value_changed flag
            \return Void 
            */
            void clear_value_changed()
            {
                _value_changed = false;
            }

            /*!
            \brief Returns the _value_changed_time_ms value (which will be 0 if the value has never been set)
            Otherwise it will be the time since boot in ms that the value was changed 
            */
            unsigned long get_value_changed_time_ms()
            {
                return _value_changed_time_ms;
            }

            /*!
            \brief Returns whether the value has been refreshed (had a value set to it) since the last call of this function.
            \arg clear_on_read is whether to set _value_refreshed to False
            \details value_refreshed is set to TRUE every time set_value() is called.
            calling get_value_refreshed clears the flag by default unless you pass in an argument to prevent it.
            */
            bool get_value_refreshed(bool clear_on_read = true)
            {
                if(! clear_on_read)
                {
                    return _value_refreshed;
                }
                bool refreshed_temp = get_value_refreshed(false);
                clear_value_refreshed();
                return refreshed_temp;
            }

            void clear_value_refreshed()
            {
                _value_refreshed = false;
            }

        protected:
            T _value;
            bool _value_refreshed = false; //Set to true every time set_value() is called. Allows us to use get_value_changed() to trigger behaviour on info being received
            bool _value_changed = false;
            unsigned long _value_refreshed_time_ms = 0;
            unsigned long _value_changed_time_ms = 0;
            
            bool _uninitialised = true;
            
    };
}
