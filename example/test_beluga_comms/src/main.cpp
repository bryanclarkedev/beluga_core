#include <Arduino.h>
//#include "beluga_device.h"

//beluga_core::device this_comms_device;
#include <sstream>
#include "beluga_debug.h"
#include "beluga_ini_reader.h"
#include "beluga_comms.h"

std::string config_file_path = "/test.ini";
beluga_utils::ini_reader this_ini(config_file_path);
beluga_core::comms this_comms_device;
std::stringstream ss;

int iter = 0;

void setup() {
  Serial.begin(115200);
  for(int i = 0; i < 5; i++)
  {
    Serial.println(5-i);
    delay(1000);
  }    
  this_ini.initialise();
  std::string comms_name("demo_comms");
  Serial.print("----Initialising comms device ");
  Serial.println(comms_name.c_str());

  auto this_ini_ptr = std::make_shared<beluga_utils::ini_reader>(this_ini); //We create a shared_ptr to an existing ini_reader
  this_comms_device.initialise(this_ini_ptr, comms_name);

  Serial.println("---printing config---");
  this_ini.print_config_to_serial();




}

void loop() {
  std::string rx_str;
  bool got_rx = this_comms_device.get_rx_msg(rx_str);
  if(got_rx)
  {
    ss.str("");
    ss << "Got an rx: " << rx_str;
    beluga_utils::debug_print(ss.str());
  }
  //We can't do tx well in the base class
  ss.str("");
  ss << "This is message #" << iter;
  this_comms_device.add_to_rx_queue(ss.str());
  //Run
  this_comms_device.run();
  
  
  ss.str("");
  ss << "Iteration " << iter << " time " << (int) (millis() / 1000) << "s";
  beluga_utils::debug_print(ss.str().c_str());
  iter++;
  delay(1000);
}
