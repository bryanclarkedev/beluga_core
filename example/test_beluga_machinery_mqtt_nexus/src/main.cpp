#include <Arduino.h>
#include "beluga_device.h"
#include "beluga_machinery.h"
#include <Wire.h>
/*
export WIFI_SSID=MyWifiName
export WIFI_PASSWORD=password123
Open HiveMQ websocket client https://www.hivemq.com/demos/websocket-client/
Subscribe to beluga/esp32/mqtt_nexus_tx
Publish to beluga/esp32/mqtt_nexus_rx: |status_led||0 OR |status_led||1

*/
beluga_core::device this_device;

beluga_core::machinery machinery_mqtt_nexus;

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

  //Need to initialise wire early or I2C devices won't run
  Wire.begin();//Can set pin mapping here if we want.

  machinery_mqtt_nexus.initialise(config_file_path, "machinery_mqtt_nexus_demo");
}

void loop() {

  machinery_mqtt_nexus.run();
  machinery_mqtt_nexus.kill_main_thread();
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
