#pragma once

#include "beluga_device.h"
#include "beluga_value.h"
#include <map>
//#include "core_enums.h"
#include "beluga_type_hasher.h"

using namespace beluga_utils;

namespace beluga_core
{
    
    /*!
    \brief A map of beluga_core::values
    \detail Some quantities involve multiple values at once.beluga_core
    e.g. a 3-axis accelerometer.
    So we create a key-value map.
    
    For simplicity we use the value_map for everything, even single-value data.
    If single-value data is used, the key can be "" 
    */
    template <typename T>
    class value_map 
    {
            public:
                //https://stackoverflow.com/questions/5056664/get-the-type-of-the-template-parameter-from-an-object-c
                typedef T type;
                
                value_map<T>(){
                    T t;
                    _this_typehash = beluga_utils::get_type_hash(t);//Typehash is used in the factory class
                    _this_typename = beluga_utils::type_name(t); // beluga_utils::get_typename(t);
                };
                
                //Copy constructor
                value_map<T>(const value_map<T> &b) = default ;
                //operator=
                //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
                value_map<T> & operator=(value_map<T> other)
                {
                    swap(*this, other); 
                    return *this;
                }

                //Swap for operator=
                friend void swap(value_map<T> & first, value_map<T> & second)
                {
                    // enable ADL (not necessary in our case, but good practice)
                    using std::swap;

                    // by swapping the members of two objects,
                    // the two objects are effectively swapped
                    swap(first._values, second._values);
                    swap(first._this_typehash, second._this_typehash);
                    swap(first._this_typename, second._this_typename);
                }


                //---------------------------------------------------
                /*!
                \brief Return the value as a beluga_core::value<T>
                \detail 
                \arg return_val is the beluga_core::value<T> that will be used to store the return value if the key is found
                \arg key_str is the key for the value that is desired; defaults to "" 
                \return a bool, True if the key string is found, False if it is not
                If returning true, the return_val will contain the value

                Beware: if checking for changes, the flag won't clear in the "real" value as this returns a copy
                */
                virtual bool get_value(value<T> & return_val, std::string key_str = "")
                {
                    for(auto iter = _values.begin(); iter != _values.end(); iter++)
                    {
                        if(iter->first == key_str)
                        {
                            return_val = _values[key_str];                            
                            return true;
                        }
                    }
                    return false;
                }

                /*
                \brief Return the value as a simple type T
                \detail 
                \arg return_val is the T that will be used to store the return value if the key is found
                \arg key_str is the key for the value that is desired; defaults to "" 
                \return a bool, True if the key string is found, False if it is not
                If returning true, the return_val will contain the value
                */
                virtual bool get_value(T & return_val, std::string s = "" )
                {
                    for(auto iter = _values.begin(); iter != _values.end(); iter++)
                    {
                        if(iter->first == s)
                        {
                            return _values[s].get_value(return_val);
                        }
                    }
                    return false;
                }

                /*!
                \brief Set the value T using key key_str

                \arg in_value is the value being set
                \arg key_str is the key to be used. It can be left blank and will default to "". The key_str MUST already exist
                in the value map or the value will not be changed
                \return a bool, True if the key_str exists and False if it does not.
                */
                virtual bool set_value(const T in_value, std::string key_str = "")
                {
                    for(auto iter = _values.begin(); iter != _values.end(); iter++)
                    {
                        if(iter->first == key_str)
                        {
                            //This name is valid
                            _values[key_str].set_value(in_value);
                            return true;
                        }
                    }
                    return false;
                }

                /*!
                \brief Create a new named value in the map
                \return A bool,  True if added successfully, False if the key already exists.
                */
                virtual bool add_new_value(std::string new_key_str = "")
                {
                    for(auto iter = _values.begin(); iter != _values.end(); iter++)
                    {
                        if(iter->first == new_key_str)
                        {
                            return false; //value already exists
                        }
                    }

                    //Add to map
                    value<T> v;
                    _values[new_key_str] = v;

                    return true;            
                }

                /*!
                \brief Retrieve the _value_changed flag from the beluga_core::value<T>
                \arg key_str is the key string of the value to retrieve, defaulting to ""
                \returns a bool, True if the value has changed since it was last read.beluga_core
                \details NOTE: hard coded to clear the change flag on read.
                */
                virtual bool get_value_changed(std::string key_str = "")
                {
                    for(auto iter = _values.begin(); iter != _values.end(); iter++)
                    {
                        if(iter->first == key_str)
                        {
                            bool clear_on_read = true;
                            return _values[key_str].get_value_changed(clear_on_read); 
                        }
                    }
                    return false;
                }

                /*!
                \brief Get a pointer to a beluga_core::value<T> 
                \arg ptr is the pointer to set with the pointer to the value
                \arg key_str is the desired key
                \return a bool, True if the key is found, False if the key is not. If True, ptr will be set to point to the value<T>
                */
               virtual bool get_value_pointer(value<T> * ptr, std::string key_str = "")
               {
                    for(auto iter = _values.begin(); iter != _values.end(); iter++)
                    {
                        if(iter->first == key_str)
                        {
                            ptr = &_values[key_str];
                            return true;
                        }
                    }
                    return false;
               }
               /*!
               \brief return the typehash of this value_map
               */
                std::size_t get_typehash()
                {
                    return _this_typehash;
                }

                /*!
               \brief return the typename of this value_map
               */
              std::string get_typename()
              {
                  return _this_typename;
              }
              
            protected:
                std::map<std::string, value<T> > _values;
                std::size_t _this_typehash;
                std::string _this_typename;

    };

    
}