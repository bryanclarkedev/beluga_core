#include <Arduino.h>
#include "beluga_machinery.h"
#include "beluga_generic_machinery.h"
#include "beluga_digital_output.h"

#include "beluga_tcp_client.h"
beluga_core::generic_machinery<beluga_core::digital_output> this_machinery;

std::string config_file_path = "/test.ini";

std::stringstream ss;

int iter = 0;

/*
Normally we would create a derived class containing all these, rather than using the generic machinery()
Then we would implement this logic in run() a bit more cleanly
*/
std::shared_ptr<beluga_core::comms> comms_ptr;
std::shared_ptr<beluga_core::tcp_client> tcp_client_ptr;
std::shared_ptr<beluga_core::device> led1_ptr;


void setup() {
  Serial.begin(115200);
  for(int i = 0; i < 5; i++)
  {
    Serial.println(5-i);
    delay(1000);
  }    
  this_machinery.initialise(config_file_path, "demo_machinery");
  this_machinery.read_config_comms();

  bool got_tcp_client = this_machinery.get_comms("tcp_client1", comms_ptr);
  if(! got_tcp_client)
  {
    while(1)
    {
      Serial.println("Error: could not get TCP client.");
      delay(1000);
    }
  }
  tcp_client_ptr = std::static_pointer_cast<beluga_core::tcp_client>(comms_ptr);
  
  bool got_led = this_machinery.get_subdevice("led1", led1_ptr);
  if(! got_led)
  {
    while(1)
    {
      Serial.println("Error: could not get led!");
      delay(1000);
    }
  }

}

void loop() 
{
  bool tcp_connected = tcp_client_ptr->is_connected();
  bool setpoint_ok = this_machinery.set_setpoint("led1", tcp_connected);

  tcp_client_ptr->run();
  led1_ptr->run();
  /*
  bool b = this_machinery.run();
  assert(b == false);
  */
  ss.str("");
  ss << "Iteration " << iter << " time " << (int) (millis() / 1000) << "s";
  Serial.println(ss.str().c_str());
  iter++;
  delay(1000);
}
