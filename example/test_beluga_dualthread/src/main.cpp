#include <Arduino.h>
#include "beluga_device.h"
#include "beluga_dualthread.h"
beluga_core::device this_device;

beluga_core::dualthread dualthread_app;

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
  //this_device.initialise(config_file_path, "demo_device");
  dualthread_app.initialise(config_file_path, "dualthread_demo");
}

void loop() {

  dualthread_app.run();
  dualthread_app.kill_main_thread();
  while(1)
  {

  }
  //dualthread_app.kill_main_thread();
  /*
  bool b = this_device.run();
  assert(b == false);
  ss.str("");
  ss << "Iteration " << iter << " time " << (int) (millis() / 1000) << "s";
  Serial.println(ss.str().c_str());
  iter++;
  delay(1000);
  */
}
