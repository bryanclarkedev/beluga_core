/*
This creates a few objects and runs a few tests.
No peripherals required.
*/
#include <Arduino.h>
#include "beluga_value_map.h"
#include <sstream>
#include <vector> 
#include "beluga_string.h"
//Value is quite basic, but can support both set_value() and get_value()
beluga_core::value_map<int> int_value_map;

std::stringstream ss;

void setup() {
  Serial.begin(115200);

  //---Test get_value()---
  beluga_core::value<int> int_value;
  bool got_val = int_value_map.get_value(int_value, "potato"); //Returns false, i unchanged as we haven't set "potato" to anything yet. add_new_value() and set_value() must be called before get_value
  ss << "Got val 1 (should be false): " << got_val;
  assert(! got_val);
  //Clear the stringstream after every use.
  Serial.println(ss.str().c_str());
  ss.str("");

  //-----Test set_value() with key 'potato'
  bool added = int_value_map.add_new_value("potato");
  bool set_val = int_value_map.set_value(7, "potato");
  ss << "Set val 1 (should be true): " << set_val;
  Serial.println(ss.str().c_str());
  ss.str("");
  //Retrieve the value that was set, using get_value(). 
  got_val = int_value_map.get_value(int_value, "potato"); //Returns true and x = 7
  assert(got_val );
  ss << "Got val 2 (should be true): " << got_val ;
  Serial.println(ss.str().c_str());
  ss.str("");

}

/*
Run the device object, print output, wait one second.
*/
void loop() {
  ss.str("");
  ss << "If you see this, the device is healthy. Press the RESET button to run initial checks.";
  Serial.println(ss.str().c_str());
  ss.str("");
  ss << " t: " << millis();
  Serial.println(ss.str().c_str());
  
  delay(1000);
}

