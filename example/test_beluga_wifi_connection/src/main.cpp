#include <Arduino.h>
#include "beluga_device.h"

#include "beluga_wifi_connection.h"

beluga_core::wifi_connection this_wifi;

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
  this_wifi.initialise(config_file_path, "wifi_connection1");
}

void loop() {
  bool b = this_wifi.run();
  //assert(b == false);
  ss.str("");
  ss << "Iteration " << iter << " Wifi connection state: " << b << " time " << (int) (millis() / 1000) << "s";
  Serial.println(ss.str().c_str());
  iter++;
  delay(1000);
}
