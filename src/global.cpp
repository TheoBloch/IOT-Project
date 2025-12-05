#include "global.h"
float glob_temperature = 0;
float glob_humidity = 0;

String WIFI_SSID="Galaxy S24 Ultra 4228";
String WIFI_PASS="ifqsllpqm";
String CORE_IOT_TOKEN="wekj6leufc7ctk8mw01l";
String CORE_IOT_SERVER="mqtt.coreiot.io";
String CORE_IOT_PORT="1883";

String ssid = "ESP32-YOUR NETWORK HERE!!!";
String password = "12345678";
String wifi_ssid = "abcde";
String wifi_password = "123456789";
boolean isWifiConnected = false;
SemaphoreHandle_t xBinarySemaphoreInternet = xSemaphoreCreateBinary();