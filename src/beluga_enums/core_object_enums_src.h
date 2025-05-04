//As explained in https://www.codeproject.com/Articles/32000/Improving-C-Enums-Adding-Serialization-Inheritance
ENUMITEM(digital_output)
ENUMITEM(analog_output)
//ENUMITEM(LED)
//As explained in https://www.codeproject.com/Articles/32000/Improving-C-Enums-Adding-Serialization-Inheritance
ENUMITEM(digital_input)
ENUMITEM(analog_input)

ENUMITEM(tcp_client)
ENUMITEM(wifi_connection)
ENUMITEM(mqtt_client)

#if ENABLE_MQTT_SERVER
ENUMITEM(mqtt_server)
#endif

ENUMITEM(wifi_manager)

ENUMITEM(deepsleep)

ENUMITEM(thread)
ENUMITEM(interthread_buffer)
ENUMITEM(machinery_thread)
ENUMITEM(dualthread)

ENUMITEM(mqtt_client_machinery)
ENUMITEM(led_machinery)

/*
ENUMITEM(Button)
ENUMITEM(LSM303_Compass_Accelerometer)
ENUMITEM(L3G_Gyro)
ENUMITEM(Digital_IR_Proximity_Sensor)
*/