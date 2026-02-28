#include "beluga_temperature_pressure_bmp280.h"
namespace beluga_core
{

    //copy-and-sep for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    void swap(temperature_pressure_bmp280 & first, temperature_pressure_bmp280& second) // nothrow
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        swap(static_cast<beluga_core::mechanism<float> &>(first), static_cast<beluga_core::mechanism<float> &>(second));

        swap(first._temperature_pressure, second._temperature_pressure);   
    }

    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    temperature_pressure_bmp280& temperature_pressure_bmp280::operator=(temperature_pressure_bmp280 other) 
    {
        swap(*this, other); 
        return *this;
    }

    bool temperature_pressure_bmp280::read_config()
    {
        bool config_ok = false;
        std::string config_val;
        config_ok = _ini_ptr->get_config_value(_config_file_section, "enable_serial_debug", &config_val );
        if(config_ok)
        {
            set_serial_debug_enabled(config_val);
        }    

        add_new_value("pressure_Pa");
        add_new_value("temperature_C");
        add_new_value("altitude_m");
        
        uint8_t status = _temperature_pressure.begin(0x76);
        if (!status) {
            Serial.println("temperature_pressure_bmp280: Could not find a valid BMP280 sensor, check wiring or try a different address!");
        }
          /* Default settings from datasheet. */
        _temperature_pressure.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

        return true;
    }

    bool temperature_pressure_bmp280::run(void * p )
    {
        float temperature_C = _temperature_pressure.readTemperature();
        float pressure_Pa = _temperature_pressure.readPressure();
        float altitude_m = _temperature_pressure.readAltitude(1013.25);//, pressure_Pa);
        set_value(temperature_C, "temperature_C");
        set_value(pressure_Pa, "pressure_Pa");
        set_value(altitude_m, "altitude_m");

        return true;        
    }

}
