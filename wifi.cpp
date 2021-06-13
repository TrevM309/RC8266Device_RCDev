#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "debug.h"

#ifndef STASSID
#define STASSID "RcCtrl1"
#define STAPSK  "thereisnospoon"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "192.168.4.1";
const uint16_t port = 3000;

ESP8266WiFiMulti WiFiMulti;

void WifiInit(void)
{
  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
  
}

void WifiProcess(void)
{
  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    Serial.println("wait 5 sec...");
    delay(5000);
    return;
  }

  // This will send the request to the server
  client.println("hello from ESP8266");

  //read back one line from server
  Serial.println("receiving from remote server");
  String line = client.readStringUntil('\r');
  Serial.println(line);

  Serial.println("closing connection");
  client.stop();

  Serial.println("wait 5 sec...");
  delay(5000);

}
