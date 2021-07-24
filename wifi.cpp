#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "debug.h"
#include "servos.h"
#include "wifi.h"

#define UDP_RX_PORT 4210
#define UDP_TX_PORT 4211
#define NUMCL 5
// 1 original proto, 2 Tranaller
const char *ssid     = "RcCtrl3";
const char *password = "password1234567";

WiFiUDP UDP;
IPAddress IP(192, 168, 4, 15);
IPAddress mask(255, 255, 255, 0);

// UDP Buffer
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

// last time (millis) since ADC was read
unsigned long last = 0;
int Vbat = 0;

// Local functions
void WifiGetData(void);
void scanWifi(int& ch);
void readAdc(void);

// Global function to be called during setup()
void WifiInit(void)
{
  int chann = 6;

  scanWifi(chann);
  dbgPrintf("\nConfiguring access point...\n");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IP, IP, mask);
  WiFi.softAP(ssid, password,chann);
  dbgPrintf("SSID:%s pass:%s\n",ssid,password);
  
  IPAddress myIP = WiFi.softAPIP();
  dbgPrintf("AP IP address: %d.%d.%d.%d\n",myIP[0],myIP[1],myIP[2],myIP[3]);

  uint8_t mac[6];
  WiFi.softAPmacAddress(mac);
  dbgPrintf("MAC address = %02x:%02x:%02x:%02x:%02x:%02x\n", 
    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  
  // Begin listening to UDP port
  UDP.begin(UDP_RX_PORT);
  dbgPrintf("Listening on UDP port %d\n", UDP_RX_PORT);
}

// Global function to be called during loop()
void WifiProcess(void)
{
  readAdc();
  WifiGetData();
}

// local function to read Battery Voltage
void readAdc(void)
{
  unsigned long Tnow = millis();
  if ((Tnow < last) || ((Tnow - last) > 1000))
  {
    int Adc;

    last = Tnow;
    Adc = analogRead(A0);
    Vbat = 1100 * Adc / 1023;
    // every second
    //dbgPrintf("ADC:%lx V:%d\n",Adc,Vbat);
  }
}

// receive data over UDP
void WifiGetData(void) 
{
  int len;
  uint8_t x;
 
  // Receive packet
  len = UDP.parsePacket();
  if (len > 0)
  {
    UDP.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    for (x = 0; x < 4; x++)
    {
      ch_pos[x] = 100 + (int8_t)packetBuffer[x];
      dbgPrintf("%d:%3d ", x, ch_pos[x]);
    }
    dbgPrintf("\n");
    //ch_pos[0] = 100 + (int8_t)packetBuffer[1];
    //ch_pos[1] = 100 + (int8_t)packetBuffer[0];
    //dbgPrintf("H:%+4d V:%+4d\n",(int8_t)packetBuffer[0],(int8_t)packetBuffer[1]);
    UDP.beginPacket(UDP.remoteIP(), UDP_TX_PORT);
    UDP.write(Vbat >> 8);
    UDP.write(Vbat & 0xff);
    UDP.endPacket();
  }
}

// scan wifi, return channel in biggest free gap
void scanWifi(int& ch)
{
  dbgPrintf("\n--------------------\n");
  dbgPrintf("scan start");

  int n = WiFi.scanNetworks();
  int used[14];
  int gap[14];
  int start[14];
  int gaps = 0;
  int x; 
  int i;
  for (i = 0; i < 14; ++i)
    used[i] = 0;
  dbgPrintf(" done\n");
  if (n == 0)
    dbgPrintf("no networks found\n");
  else
  {
    dbgPrintf(" %d networks found:\n",n);
    for (i = 0; i < n; ++i)
    {
      ch = WiFi.channel(i);
      if (ch <= 14)
        used[ch-1] = 1;
      dbgPrintf("  %d: ch%d: rssi%d: ssid'%s' %c\n",
        i+1,ch,WiFi.RSSI(i),WiFi.SSID(i),(WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
    }
  }
  // array used now contains which channels are used
  for (i=0; i < 14; i++)
  {
    if (used[i] == 0)
    {
       start[gaps]=i+1;
       for(gap[gaps] = 0; i < 14 && used[i] == 0; i++, gap[gaps]++)
        ;
       gaps++;
    }
  }
  dbgPrintf(" %d gaps\n",gaps);
  for (i=0,x=0; i < gaps; i++)
  {
    if (gap[i] > x)
      x = gap[i];
    dbgPrintf(" %d ch:%d size:%d\n",i,start[i],gap[i]);
  }
  dbgPrintf(" Max gap:%d\n",x);
  for (i = 0; i < 14 && gap[i] < x; i++)
    ;
  ch = start[i] + (gap[i]/2);
  dbgPrintf(" Channel = %d\n",ch);
}
