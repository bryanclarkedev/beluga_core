#include <Arduino.h>
#include "beluga_device.h"
#include "beluga_machine.h"
#include "beluga_deepsleep.h"
#include "beluga_digital_output.h"
#include "beluga_bmp280_temperature_pressure.h"
#include "beluga_lipo_fuel_gauge.h"
#include <iomanip> // Required for std::setprecision 

beluga_core::machine this_machine;

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
  try{
  this_machine.initialise(config_file_path, "solar_sensor");
  }
  catch(...){
    Serial.println("Problem!");
  }
}

void loop() {
  unsigned long time_now_s = (int)( millis() / 1000);
  bool b = this_machine.run();

  /*
  We assume that the subdevice name and type are known to the programmer. 
  It's probably possible to write something generic that scrapes the .ini but difficult to do something useful with that
  Using name and type we do a static cast to the specific shared_ptr type.
  */
  std::string led_name = "status_led";
  int led_setpoint = 1;
  if( time_now_s % 2 == 0)
  {
    led_setpoint = 0;
  }
  try{
    bool set_val = this_machine.set_setpoint(led_name, led_setpoint);
  }
  catch(...)
  {
    Serial.println("Bad LED");
  }
  #if 1
  std::string temperature_pressure_name = "bmp280";
  float pressure_Pa, temperature_C, altitude_m;
  bool got_pressure_ok = this_machine.get_state(temperature_pressure_name, pressure_Pa, "pressure_Pa");
  bool got_temp_ok = this_machine.get_state(temperature_pressure_name, temperature_C, "temperature_C");
  bool got_altitude_ok = this_machine.get_state(temperature_pressure_name, altitude_m, "altitude_m");
  ss.str("");
  ss << "Temperature: " << std::setprecision(2) << temperature_C << " C";
  Serial.println(ss.str().c_str());
  #endif


  std::string button_name = "button3";
  bool button_state;
  bool got_button_ok = this_machine.get_state(button_name, button_state);
  ss.str("");
  ss << "Button state: " << button_state;
  Serial.println(ss.str().c_str());


#if 0
  std::string fuel_gauge_name = "fuel_gauge";
  float voltage_V, percentage;
  try{
  bool got_voltage_ok = this_machine.get_state(fuel_gauge_name, voltage_V, "voltage_V");
  }catch(const std::exception& e)
  {
   ss.str("");
   ss <<  e.what() << '\n';
    Serial.println(ss.str().c_str());
    
  }
  bool got_percentage_ok = this_machine.get_state(fuel_gauge_name, percentage, "percentage");
    ss.str("");
  ss << "Voltage: " << std::setprecision(2) << voltage_V << " V | Percent: ";
  ss  << std::setprecision(1) << percentage;
  Serial.println(ss.str().c_str());
  
  #endif

  #if 0
  std::string rtc_name = "rtc";
  int minutes, hours;
  try
  {
    bool got_rtc_ok = this_machine.get_state(rtc_name, minutes, "minute" );
    bool got_rtc_ok2 = this_machine.get_state(rtc_name, hours, "hour" );

    ss.str("");
    ss << "hours: " << hours<<  " minute: " << minutes;
    Serial.println(ss.str().c_str());
    ss.str("");

  }
  catch(const std::exception& e)
  {
   ss.str("");
   ss <<  e.what() << '\n';
    Serial.println(ss.str().c_str());
    }
  

  #endif

  ss.str("");
  ss << "Iteration " << iter << " time " << time_now_s << "s";
  Serial.println(ss.str().c_str());


  iter++;
  delay(1000);
}
