#include "beluga_li_batt_fuel_gauge_max17043.h"
#include <Wire.h>
namespace beluga_core
{

    //copy-and-sep for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    void swap(li_batt_fuel_gauge& first, li_batt_fuel_gauge& second) // nothrow
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        swap(static_cast<beluga_core::mechanism<float> &>(first), static_cast<beluga_core::mechanism<float> &>(second));

        swap(first._fuel_gauge, second._fuel_gauge);   
    }

    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    li_batt_fuel_gauge& li_batt_fuel_gauge::operator=(li_batt_fuel_gauge other) 
    {
        swap(*this, other); 
        return *this;
    }

    bool li_batt_fuel_gauge::read_config()
    {
        bool config_ok = false;
        std::string config_val;
        config_ok = _ini_ptr->get_config_value(_config_file_section, "enable_serial_debug", &config_val );
        if(config_ok)
        {
            set_serial_debug_enabled(config_val);
        }    

        add_new_value("voltage_V");
        add_new_value("percentage");
        
        Wire.begin();
        try
        {
        _fuel_gauge.reset();
        }
        catch(const std::exception& e)
        {
            beluga_utils::debug_print_loop_forever("li_batt_fuel_gauge: Could not initialise!");
        }
        
        _fuel_gauge.quickStart();
        /*
        if(_fuel_gauge.begin() != 0) 
        {
            return false;
        }
        */

        
        return true;
    }


    bool li_batt_fuel_gauge::run(void * p )
    {

        //float millivolt_to_volt = 0.001;
        float voltage_V =  _fuel_gauge.getVCell();// * millivolt_to_volt ; //Raw read is in mV, we work in V
        set_value(voltage_V, "voltage_V");
        float percentage = _fuel_gauge.getSoC(); //State of charge
        set_value(percentage, "percentage");

        return true;        
    }
}