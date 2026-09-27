#include <Arduino.h>
#include "beluga_analog_input.h"
#//include "beluga_digital_output.h"

beluga_core::analog_input a_in;
//beluga_core::digital_output d_out;

std::string config_file_path = "/test.ini";

std::stringstream ss;

/*
For this test, use a DuPont wire to connect IO8 to 3V3. 
Connecting it should turn on the builtin LED. 
Disconnecting it should turn the LED off
*/
void setup() {
  Serial.begin(115200);
  for(int i = 0; i < 5; i++)
  {
    Serial.println(5-i);
    delay(1000);
  }    

  a_in.initialise(config_file_path, "analog_input1");
  //d_out.initialise(config_file_path, "digital_output1"); 
  
}

void loop() {
  
  a_in.run();
  bool a_ok;
  int16_t a_val;
  a_ok = a_in.get_value(a_val);
  ss.str("");
  if(a_ok)
  {
    ss << "Analog in: " << a_val;
  }else{
    ss << "Button error";
  }
  Serial.println(ss.str().c_str());

  #if 0
  if(d_ok)
  {
    bool out_ok = d_out.set_value(d_val);
    d_out.run();
  }
  #endif
  Serial.println(millis());
  delay(1000);
}

