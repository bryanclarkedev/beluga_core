#include <Arduino.h>
#include "beluga_device.h"

#include "beluga_deepsleep.h"

beluga_core::deepsleep this_deepsleep;

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
  this_deepsleep.initialise(config_file_path, "demo_deepsleep");
  this_deepsleep.print_wakeup_reason();
}

void loop() {
  bool b = this_deepsleep.run();
  //assert(b == false);
  ss.str("");
  ss << "Iteration " << iter << " time " << (int) (millis() / 1000) << "s";
  Serial.println(ss.str().c_str());
  iter++;
  delay(1000);
}
