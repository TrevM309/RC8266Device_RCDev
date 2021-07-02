/*
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
