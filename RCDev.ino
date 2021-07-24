/*
 * Use ESP8266 as RC, this is the Remote Controlled Device
 * It is WiFi server, listening for UDP messages from Controller (client) with H & V percentages
 * Also responds with UDP containing current Battery Voltage
*/
#include "servos.h"
#include "debug.h"
#include "wifi.h"

void setup() 
{
  dbgInit();
  WifiInit();
  servo_init();
}


void loop() 
{
  WifiProcess();
  servo_process();
}
