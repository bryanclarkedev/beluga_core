#include <Arduino.h>
#include "beluga_lipo_fuel_gauge.h"

beluga_core::lipo_fuel_gauge this_fuel_gauge;

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
  this_fuel_gauge.initialise(config_file_path, "fuel_gauge");
}

void loop() {
  bool b = this_fuel_gauge.run();
  if(b)
  {
    float V;
    bool got_V_ok = this_fuel_gauge.get_value(V, "voltage_V");
    float percent;
    bool got_percent_ok = this_fuel_gauge.get_value(percent, "percentage");
    ss.str("");
    if(got_V_ok && got_percent_ok)
    {
      ss << "Battery status: " << V << " V, " << percent << "%";
      Serial.println(ss.str().c_str());
    }
  }
  ss.str("");
  ss << "Iteration " << iter << " time " << (int) (millis() / 1000) << "s";
  Serial.println(ss.str().c_str());
  iter++;
  delay(1000);
}
