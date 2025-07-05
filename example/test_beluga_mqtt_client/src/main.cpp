#include <Arduino.h>
#include "beluga_device.h"
#include "beluga_wifi_connection.h"
#include "beluga_mqtt_client.h"
beluga_core::mqtt_client this_mqtt;
beluga_core::wifi_connection this_wifi;
std::string config_file_path = "/test.ini";

std::stringstream ss;

int iter = 0;
long lastMsg = 0;
float time_meas_ms = 0;
const int ledPin = 2;// LED_BUILTIN; //Pin 2

void setup() {
  Serial.begin(115200);
  for(int i = 0; i < 5; i++)
  {
    Serial.println(5-i);
    delay(1000);
  }    
  //We use the LED to indicate RX message state
  pinMode(LED_BUILTIN, OUTPUT);
  this_wifi.initialise(config_file_path, "wifi_connection1");
  this_mqtt.initialise(config_file_path, "mqtt_client");
}


void handle_rx()
{
    std::list<std::string> this_rx_list;
    std::string this_topic_str = "beluga/esp32/output";
    this_mqtt.get_rx_queue(this_rx_list, this_topic_str);
    if(this_rx_list.size() > 0)
    {
      std::stringstream ss;
      ss << "Rx topic " << this_topic_str << " N_rx: " << this_rx_list.size();
      Serial.println(ss.str().c_str());
        
      for(auto msg_iter = this_rx_list.begin(); msg_iter != this_rx_list.end(); msg_iter++)
      {
        std::string msg_str = *msg_iter;
        Serial.print("Setting output to: ");
        if(msg_str == "on")
        {
          Serial.println("on");
          digitalWrite(ledPin, HIGH);
        }  else if(msg_str == "off"){
            Serial.println("off");
            digitalWrite(ledPin, LOW);
        }else{
            Serial.println("Bad RX: ");
            Serial.println(msg_str.c_str());
        }
      }
    }

}

void loop() {


    //ss <<"lastMsg: " << lastMsg << " now: " << now << " dt_ms " << dt_ms << " elapsed: " << elapsed;
    //Serial.println(ss.str().c_str());
    handle_rx();

    long now = millis();
    long dt_ms = now - lastMsg;
    std::stringstream ss;
    bool elapsed = dt_ms > 5000;

    if (elapsed)
    {
      //Serial.println(elapsed);
      lastMsg = now;
      time_meas_ms = now;   
      
      // Convert the value to a char array
      char tempString[15];
      dtostrf(time_meas_ms, 1, 2, tempString);
      Serial.print("Publishing time_meas_ms: ");
      Serial.println(tempString);
      //client.publish("esp32/time_meas_ms", tempString);
      this_mqtt.send_to_mqtt("esp32/time_meas_ms", tempString);
    }


  this_mqtt.run();  
  delay(1);
  return;


}
