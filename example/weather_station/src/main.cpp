/*
To interact with the system
Publish to weather_station/control/led_setpoint either "on" or "off" ->  built in LED will turn on and off
Subscribe to weather_station/sense/# -> see published data
*/

#include <Arduino.h>
#include "beluga_mqtt_client.h"
#include "beluga_li_batt_fuel_gauge_max17043.h"
#include "beluga_rtc_sd2405.h"
#include "beluga_temperature_pressure_bmp280.h"
#include "beluga_deepsleep.h"
#include <WiFi.h>
beluga_core::mqtt_client this_mqtt;
beluga_core::li_batt_fuel_gauge this_fuel_gauge;
beluga_core::rtc_sd2405 this_rtc;
beluga_core::temperature_pressure_bmp280 this_temperature_pressure;
beluga_core::deepsleep this_deepsleep;

std::string config_file_path = "/test.ini";

std::stringstream ss;

int iter = 0;
const int ledPin = 2;



void handle_rx()
{
    std::list<std::string> this_rx_list;
    std::string this_topic_str = "weather_station/control/led_setpoint";
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


void setup() {
  Serial.begin(115200);
  for(int i = 0; i < 5; i++)
  {
    Serial.println(5-i);
    delay(1000);
  }    
  Wire.begin();

  this_fuel_gauge.initialise(config_file_path, "fuel_gauge");
  this_mqtt.initialise(config_file_path, "mqtt_client");
  this_rtc.initialise(config_file_path, "rtc");
  this_temperature_pressure.initialise(config_file_path, "bmp280");
  this_deepsleep.initialise(config_file_path, "deepsleep");

  pinMode(ledPin, OUTPUT);
}

void loop() 
{
  this_rtc.run();
  //-----------------------MQTT RX-----------------------
  this_mqtt.run();  
  handle_rx();

  //------------------MQTT TX----------------------
  //--------------Time--------------------
  float time_meas_ms = millis();
  char buffer[15];
  dtostrf(time_meas_ms, 1, 2, buffer);
  Serial.print("time_meas_ms: ");
  Serial.println(buffer);
  this_mqtt.add_to_tx_queue(buffer, "weather_station/sense/uptime_ms");
  std::string datetime_str;
  bool datetime_ok = this_rtc.get_date_time_string(datetime_str);
  if(datetime_ok)
  {
    Serial.println(datetime_str.c_str());
    this_mqtt.add_to_tx_queue(datetime_str, "weather_station/sense/datetime" );
  }
  //-----------------------Fuel gauge------------------
  bool b2 = this_fuel_gauge.run();
  if(b2)
  {
    float V;
    bool got_V_ok = this_fuel_gauge.get_value(V, "voltage_V");
    float percent;
    bool got_percent_ok = this_fuel_gauge.get_value(percent, "percentage");
    ss.str("");
    if(got_V_ok && got_percent_ok)
    {
      ss << "Battery status: " << V << " V, " << percent << "%";
      Serial.println(ss.str().c_str());

      this_mqtt.add_to_tx_queue(ss.str(), "weather_station/sense/battery_status");
    }else{
      this_mqtt.add_to_tx_queue("error", "weather_station/sense/battery_status");
    }
  }

  //---------------------Temperature pressure------------------------
  bool b = this_temperature_pressure.run();
  if(b)
  {
    float temperature_C;
    bool got_temperature_C_ok = this_temperature_pressure.get_value(temperature_C, "temperature_C");
    float pressure_Pa;
    bool got_pressure_ok = this_temperature_pressure.get_value(pressure_Pa, "pressure_Pa");
    ss.str("");
    if(got_temperature_C_ok && got_pressure_ok)
    {
      ss << "Temperature: " << temperature_C << " (deg C), Pressure " << pressure_Pa << " (Pa)";
      Serial.println(ss.str().c_str());
      this_mqtt.add_to_tx_queue(ss.str(), "weather_station/sense/temperature_pressure");
    }else{
      this_mqtt.add_to_tx_queue("error", "weather_station/sense/temperature_pressure");
    }
    
  }

  //RSSI
  try
  {
    if((WiFi.status() == WL_CONNECTED))
    {
      long rssi = WiFi.RSSI();
      ss.str("");
      ss << rssi;
      this_mqtt.add_to_tx_queue(ss.str(), "weather_station/sense/wifi_rssi");
      ss.str("");
    }else{
      Serial.println("No wifi!");
    }
  }
  catch(...)
  {
    ;
  }

  ss.str("");
  ss << "Iteration " << iter << " time " << (int) (millis() / 1000) << "s";
  Serial.println(ss.str().c_str());
  iter++;


  delay(1000);
  //Deep sleep
  bool sleep_immediately = this_deepsleep.run();
  if(!sleep_immediately)
  {
        uint16_t wake_iterations_count;
        bool got_dt_ok = this_deepsleep.get_value(wake_iterations_count, "wake_iterations_count" );
        if(got_dt_ok)
        {
          ss.str("");
          ss << "deepsleep_count: " << wake_iterations_count;
          Serial.println(ss.str().c_str());
          ss.str("");
          ss << wake_iterations_count;
          this_mqtt.add_to_tx_queue(ss.str(), "weather_station/sense/deepsleep_count");
        }else{
          this_mqtt.add_to_tx_queue("error", "weather_station/sense/deepsleep_count");
        }
  }

}
