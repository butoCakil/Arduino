// #include <ESP8266WiFi.h>
// #include <WiFiClient.h>
// #include <ESP8266WebServer.h>
// #include <ESP8266mDNS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <WiFiManager.h>
#include "index.h"

#define RST_AP 19

WebServer server(80);

const int pinLED = 21;
            
void setup() {
  pinMode(pinLED, OUTPUT);
  pinMode(RST_AP, INPUT_PULLUP);
  digitalWrite(pinLED, HIGH);

  Serial.begin(115200);

  WiFiManager wifiManager;

  // reset wifi
  if (digitalRead(RST_AP) == 0) {
    wifiManager.resetSettings();
  }

  wifiManager.autoConnect("digitalTest");

  Serial.println("Nyambung Coy...");

  if (MDNS.begin("digi")) {
    Serial.println("MDNS responer started");
  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HOMEPAGE);
  server.on("/about", ABOUT);
  server.on("/on", onLED);
  server.on("/off", offLED);
  server.begin();
}

void loop() {
  // MDNS.update();
  server.handleClient();
}
