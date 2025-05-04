#include <Arduino.h>
#include "beluga_digital_input.h"
#include "beluga_digital_output.h"

beluga_core::digital_input d_in;
beluga_core::digital_output d_out;

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

  d_in.initialise(config_file_path, "digital_input1");
  d_out.initialise(config_file_path, "digital_output1"); 
  
}

void loop() {
  
  d_in.run();
  bool d_ok, d_val;
  d_ok = d_in.get_value(d_val);
  ss.str("");
  if(d_ok)
  {
    ss << "Button: " << d_val;
  }else{
    ss << "Button error";
  }
  Serial.println(ss.str().c_str());

  if(d_ok)
  {
    bool out_ok = d_out.set_value(d_val);
    d_out.run();
  }
  Serial.println(millis());
  delay(1000);
}

