#include <Arduino.h>
#include "beluga_device.h"

#include "beluga_wifi_connection.h"
#include "beluga_tcp_client.h"

beluga_core::wifi_connection this_wifi;
beluga_core::tcp_client this_tcp_client;

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
  this_tcp_client.initialise(config_file_path, "tcp_client1");
}

void loop() {
  bool b = this_wifi.run();
  
  this_tcp_client.run();
  if(this_tcp_client.has_rx()){
    std::string rx_str = this_tcp_client.get_rx();
    bool add_newline = false;
    beluga_utils::debug_print("Got: ", add_newline);
    beluga_utils::debug_print(rx_str.c_str());
  }
  //assert(b == false);
  ss.str("");
  ss << "Iteration " << iter << " Wifi connection state: " << b << " time " << (int) (millis() / 1000) << "s";
  Serial.println(ss.str().c_str());
  
  iter++;
  delay(1000);
}
