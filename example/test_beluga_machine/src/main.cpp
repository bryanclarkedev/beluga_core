#include <Arduino.h>
#include "beluga_device.h"
#include "beluga_machine.h"
#include "beluga_digital_input.h"
#include "beluga_digital_output.h"

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
  this_machine.initialise(config_file_path, "demo_machine");
}

void loop() {
  bool b = this_machine.run();

  /*
  We assume that the subdevice name and type are known to the programmer. 
  It's probably possible to write something generic that scrapes the .ini but difficult to do something useful with that
  Using name and type we do a static cast to the specific shared_ptr type.
  */
  bool in_val; 
  std::string d_in_name = "digital_input1";
  bool got_val = this_machine.get_state(d_in_name, in_val);
  
  //Use d_in to drive d_out
  std::string d_out_name = "digital_output1";
  bool set_val = this_machine.set_setpoint(d_out_name, in_val);
  
  Serial.print("Button state: ");
  Serial.println((int) in_val);

  float temperature_C;
  std::string bmp280_name = "bmp280";
  std::string temperature_key = "temperature_C";
  bool temp_val = this_machine.get_state(bmp280_name, temperature_C, temperature_key);
  Serial.print("Temperature: ");
  Serial.println(temperature_C);

  ss.str("");
  ss << "Iteration " << iter << " time " << (int) (millis() / 1000) << "s";
  Serial.println(ss.str().c_str());
  iter++;
  delay(1000);
}
