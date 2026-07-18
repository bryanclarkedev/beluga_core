#include <Arduino.h>
#include "beluga_device.h"
#include "beluga_machine.h"
//#include "beluga_deepsleep.h"
#include "beluga_digital_output.h"
#include "beluga_temperature_pressure_bmp280.h"
#include "beluga_rtc_sd2405.h"
#include "beluga_mqtt_client.h"

//#include "U8g2lib.h"
#include <Wire.h>
#include <SPI.h>

//#include "beluga_li_batt_fuel_gauge_max17043.h"
#include <iomanip> // Required for std::setprecision 

beluga_core::machine this_machine;
std::string config_file_path = "/test.ini";

std::stringstream ss;

int iter = 0;
long lastMsg = 0;
float time_meas_ms = 0;

#if ESP32C3
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, 6, 5);
int width = 72;
int height = 40;
#endif

void setup() {
  Serial.begin(115200);
  for(int i = 0; i < 5; i++)
  {
    Serial.println(5-i);
    delay(1000);
  }    
  try{

  Wire.begin();//Can set pin mapping here if we want.
  this_machine.initialise(config_file_path, "esp32_in_a_box");
  std::shared_ptr<beluga_core::mqtt_client> mqtt_client_ptr = std::make_shared<beluga_core::mqtt_client>();
  bool got_mqtt = this_machine.get_subdevice("mqtt_client", mqtt_client_ptr );
  assert(got_mqtt);
  

  #if ESP32C3
     u8g2.begin();
     u8g2.setContrast(255); // set contrast to maximum 
     u8g2.setBusClock(400000); //400kHz I2C 
     u8g2.setFont(u8g2_font_ncenB10_tr);
  #endif
  }
  catch(...){
    beluga_utils::debug_print_loop_forever("Problem initialising!");
  }
  
}


void handle_rx()
{
    std::list<std::string> this_rx_list;
    std::string this_topic_str = "beluga/esp32_in_a_box/speaker_setpoint";
    
    std::shared_ptr<beluga_core::mqtt_client> this_mqtt_ptr = std::make_shared<beluga_core::mqtt_client>();
    bool got_mqtt = this_machine.get_subdevice("mqtt_client", this_mqtt_ptr );
    assert(got_mqtt);
    
    this_mqtt_ptr->get_rx_queue(this_rx_list, this_topic_str);
    if(this_rx_list.size() > 0)
    {
      std::stringstream ss;
      ss << "Rx topic " << this_topic_str << " N_rx: " << this_rx_list.size();
      Serial.println(ss.str().c_str());
      
      #if 0
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
      #endif

    }

}



void loop() {
  unsigned long time_now_s = (int)( millis() / 1000);
  bool b = this_machine.run();

  std::shared_ptr<beluga_core::mqtt_client> this_mqtt_ptr = std::make_shared<beluga_core::mqtt_client>();
  bool got_mqtt = this_machine.get_subdevice("mqtt_client", this_mqtt_ptr );
  assert(got_mqtt);
    
  //bool b2 = this_mqtt.run();
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
  //bool got_altitude_ok = this_machine.get_state(temperature_pressure_name, altitude_m, "altitude_m");
  ss.str("");
  ss << "Temperature: " << std::setprecision(2) << temperature_C << " C";
  Serial.println(ss.str().c_str());
  #endif

  std::string button_name = "button1";
  bool button_state;
  bool got_button_ok = this_machine.get_state(button_name, button_state);
  ss.str("");
  ss << "Button state: " << button_state;
  Serial.println(ss.str().c_str());

#if 1
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

  #if 1
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

#if 1
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
      char buffer[15];
      dtostrf(time_meas_ms, 1, 2, buffer);
      Serial.print("Publishing time_meas_ms: ");
      Serial.println(buffer);
      //client.publish("esp32/time_meas_ms", buffer);
      this_mqtt_ptr->add_to_tx_queue(buffer, "beluga/esp32_in_a_box/demo_tx");
    }
#endif


#if ESP32C3
    u8g2.clearBuffer(); // clear the internal memory
    u8g2.drawFrame(0, 0, width, height); //draw a frame around the border
    u8g2.setCursor(15, 25);
    u8g2.printf("%dx%d", width, height);
    u8g2.sendBuffer(); // transfer internal memory to the display
#endif
  iter++;
  delay(1000);
}
