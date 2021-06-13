/*
    This sketch sends a string to a TCP server, and prints a one-line response.
    You must run a TCP server in your local network.
    For example, on Linux you can use this command: nc -v -l 3000
*/
#include "debug.h"
#include "wifi.h"

void setup() 
{
  dbgInit();
  WifiInit();
}


void loop() 
{
  WifiProcess();
}
