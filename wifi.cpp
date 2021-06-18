#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "debug.h"
#include "servos.h"
#include "wifi.h"

const char *ssid     = "RcCtrl1";
const char *password = "password1234567";

//WiFiServer server(80);
WiFiUDP UDP;
IPAddress IP(192,168,4,15);
IPAddress mask = (255, 255, 255, 0);
#define UDP_PORT 4210

// UDP Buffer
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

void WifiInit(void)
{
  dbgPrintf("\nConfiguring access point...\n");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IP, IP, mask);
  WiFi.softAP(ssid, password);
  
  IPAddress myIP = WiFi.softAPIP();
  dbgPrintf("AP IP address: %d.%d.%d.%d\n",myIP[0],myIP[1],myIP[2],myIP[3]);

  uint8_t mac[6];
  WiFi.softAPmacAddress(mac);
  dbgPrintf("MAC address = %02x:%02x:%02x:%02x:%02x:%02x\n", 
    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  
  // Begin listening to UDP port
  UDP.begin(UDP_PORT);
  dbgPrintf("Listening on UDP port %d\n", UDP_PORT);
  //server.begin();
  //dbgPrintf("Server started\n");
}

void WifiGetData() 
{
  int len;
 
  // Receive packet
  len = UDP.parsePacket();
  if (len > 0)
  {
    UDP.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    ch_pos[0] = 100 + (int8_t)packetBuffer[1];
    ch_pos[1] = 100 + (int8_t)packetBuffer[0];
    dbgPrintf("H:%+4d V:%+4d\n",(int8_t)packetBuffer[0],(int8_t)packetBuffer[1]);
  }
}

void WifiProcess(void)
{
  WifiGetData();
}
