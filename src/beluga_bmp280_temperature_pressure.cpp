#include "beluga_bmp280_temperature_pressure.h"
namespace beluga_core
{

        //copy-and-sep for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    void swap(bmp280_temperature_pressure & first, bmp280_temperature_pressure& second) // nothrow
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        swap(static_cast<beluga_core::mechanism<float> &>(first), static_cast<beluga_core::mechanism<float> &>(second));

        swap(first._pressure_temperature, second._pressure_temperature);   
    }

    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    bmp280_temperature_pressure& bmp280_temperature_pressure::operator=(bmp280_temperature_pressure other) 
    {
        swap(*this, other); 
        return *this;
    }
    #if 0
    void bmp280_temperature_pressure::printLastOperateStatus(DFRobot_BMP280_IIC::eStatus_t eStatus)
    {
    switch(eStatus) {
    case DFRobot_BMP280_IIC::eStatusOK:    Serial.println("everything ok"); break;
    case DFRobot_BMP280_IIC::eStatusErr:   Serial.println("unknow error"); break;
    case DFRobot_BMP280_IIC::eStatusErrDeviceNotDetected:    Serial.println("device not detected"); break;
    case DFRobot_BMP280_IIC::eStatusErrParameter:    Serial.println("parameter error"); break;
    default: Serial.println("unknow status"); break;
    }
    }
    #endif
    bool bmp280_temperature_pressure::read_config()
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
        #if 0
        _pressure_temperature.reset();
        while(_pressure_temperature.begin() != DFRobot_BMP280_IIC::eStatusOK) {
            Serial.println("bmp begin faild");
           // printLastOperateStatus(_pressure_temperature.lastOperateStatus);
            delay(2000);
        }
        #endif
        
        uint8_t status = _pressure_temperature.begin(0x76);
        if (!status) {
            Serial.println("bmp280_temperature_pressure: Could not find a valid BMP280 sensor, check wiring or try a different address!");
        }
          /* Default settings from datasheet. */
  _pressure_temperature.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */


        return true;
    }


    bool bmp280_temperature_pressure::run(void * p )
    {

        float temperature_C = _pressure_temperature.readTemperature();
        float pressure_Pa = _pressure_temperature.readPressure();
        float altitude_m = _pressure_temperature.readAltitude(1013.25);//, pressure_Pa);
        set_value(temperature_C, "temperature_C");
        set_value(pressure_Pa, "pressure_Pa");
        set_value(altitude_m, "altitude_m");

        return true;        
    }

}
