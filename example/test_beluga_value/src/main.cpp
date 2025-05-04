/*
This creates a few objects and runs a few tests.
No peripherals required.
*/
#include <Arduino.h>
#include "beluga_device.h"
#include "beluga_value.h"


//Value is quite basic, but can support both set_value() and get_value()
beluga_core::value<int> int_value;
beluga_core::value<float> float_value;


std::stringstream ss;

void setup() {
  Serial.begin(115200);

  //---Test get_value()---
  int x = 0;
  bool got_val = int_value.get_value(x); //Returns false, x unchanged as we haven't set int_value to anything yet.
  ss << "Got val 1: " << got_val;
  assert(!got_val);
  //Clear the stringstream after every use.
  Serial.println(ss.str().c_str());
  ss.str("");

  //-----Test set_value()
  int_value.set_value(7);
  //Retrieve the value that was set, using get_value(). 
  got_val = int_value.get_value(x); //Returns true and x = 7
  assert(got_val && (x == 7));
  ss << "Got val 2: " << got_val << " x: " << x;
  Serial.println(ss.str().c_str());
  ss.str("");

  //---A bunch of tests for get_value_changed()/get_value_refreshed()
  bool changed = int_value.get_value_changed(); //changed = true 
  assert(changed);
  bool refreshed = int_value.get_value_refreshed(); //refreshed = true 
  assert(refreshed);
  changed = int_value.get_value_changed(); //changed = false because reading it wipes the flag by default
  refreshed = int_value.get_value_refreshed(); //refreshed = false because reading it wipes the flag by default
  assert(!refreshed);
  int_value.set_value(7);
  changed = int_value.get_value_changed(); //changed = false because we set it to the same value
  refreshed = int_value.get_value_refreshed(); //refreshed = true 
  assert(refreshed);
  refreshed = int_value.get_value_refreshed(); //refreshed = false because reading it wipes the flag by default
  assert(!refreshed);

  int_value.set_value(13);
  changed = int_value.get_value_changed(); //changed = true 
  assert(changed);
  changed = int_value.get_value_changed(); //changed = false because reading it wipes the flag by default
  assert(!changed);
  refreshed = int_value.get_value_refreshed(); //refreshed = true 
  assert(refreshed);
  refreshed = int_value.get_value_refreshed(); //refreshed = false because reading it wipes the flag by default
  assert(!refreshed);

  int_value.set_value(99);
  bool clear_on_read = false;
  changed = int_value.get_value_changed(clear_on_read); //changed = true 
  assert(changed);
  changed = int_value.get_value_changed(clear_on_read); //changed = true because we used clear_on_read=false
  assert(changed);
  refreshed = int_value.get_value_refreshed(clear_on_read); //refreshed = true 
  assert(refreshed);
}

/*
Run the device object, print output, wait one second.
*/
void loop() {
  ss.str("");
  ss << " t: " << millis();
  Serial.println(ss.str().c_str());
  
  delay(1000);
}

