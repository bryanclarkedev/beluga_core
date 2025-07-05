#include <Arduino.h>
#include "beluga_bmp280_temperature_pressure.h"
#include <sstream>
beluga_core::bmp280_temperature_pressure this_temperature_pressure;

std::string config_file_path = "/test.ini";

std::stringstream ss;

int iter = 0;

void setup() {
  Serial.begin(115200);
  for(int i = 0; i < 5; i++)
  {
    Serial.println(5-i);
    delay(1000);
  }    
  this_temperature_pressure.initialise(config_file_path, "bmp280");
}

void loop() {
  bool b = this_temperature_pressure.run();
  if(b)
  {
    float temperature_C;
    bool got_temperature_C_ok = this_temperature_pressure.get_value(temperature_C, "temperature_C");
    float pressure_Pa;
    bool got_pressure_ok = this_temperature_pressure.get_value(pressure_Pa, "pressure_Pa");
    ss.str("");
    if(got_temperature_C_ok && got_pressure_ok)
    {
      ss << "Temperature: " << temperature_C << " (deg C), Pressure " << pressure_Pa << " (Pa)";
      Serial.println(ss.str().c_str());
    }
  }
  
  ss.str("");
  ss << "Iteration " << iter << " time " << (int) (millis() / 1000) << "s";
  Serial.println(ss.str().c_str());
  iter++;
  delay(1000);
}
