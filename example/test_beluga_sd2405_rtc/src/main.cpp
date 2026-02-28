#include <Arduino.h>
//#include "beluga_device.h"

//beluga_core::device this_rtc;
#include <sstream>
#include "beluga_debug.h"
#include "beluga_ini_reader.h"
#include "beluga_sd2405_rtc.h"

std::string config_file_path = "/test.ini";
beluga_utils::ini_reader this_ini(config_file_path);
beluga_core::sd2405_rtc this_rtc;
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

  Wire.begin();//Can set pin mapping here if we want.

  std::string device_name("rtc");
  Serial.print("----Initialising device ");
  Serial.println(device_name.c_str());

  auto this_ini_ptr = std::make_shared<beluga_utils::ini_reader>(this_ini); //We create a shared_ptr to an existing ini_reader
  this_rtc.initialise(this_ini_ptr, device_name);

  Serial.println("---printing config---");
  this_ini.print_config_to_serial();
  this_ini.clear();


}

void loop() {
  bool b = this_rtc.run();
  //assert(b == false);
  ss.str("");
  ss << "Iteration " << iter << " time " << (int) (millis() / 1000) << "s";
  std::string date_time_str;
  bool date_time_ok = this_rtc.get_date_time_string(date_time_str);
  if(date_time_ok)
  {
    ss << " DateTime String From RTC: " << date_time_str;
  }
  beluga_utils::debug_print(ss.str().c_str());
  iter++;
  delay(1000);
}
