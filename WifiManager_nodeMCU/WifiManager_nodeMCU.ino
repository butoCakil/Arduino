#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include "index.h"

#define RST_AP D5

// const char* ssid = "INTRUKTUR-TAV-2.4G";
// const char* password = "skanebabisa1";

ESP8266WebServer server(80);

const int pinLED = LED_BUILTIN;

void setup() {
  pinMode(pinLED, OUTPUT);
  pinMode(RST_AP, INPUT_PULLUP);
  digitalWrite(pinLED, HIGH);

  Serial.begin(115200);
  // mode STA atau AP (hotspot)
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);
  // Serial.println("");

  // while(WiFi.status() != WL_CONNECTED){
  //   delay(500);
  //   Serial.print(".");
  // }

  WiFiManager wifiManager;

  // reset wifi
  if (digitalRead(RST_AP) == 0) {
    wifiManager.resetSettings();
  }

  wifiManager.autoConnect("bendzone");

  Serial.println("Nyambung Coy...");

  if (MDNS.begin("ben")) {
    Serial.println("MDNS responer started");
  }

  // Serial.println("");
  // Serial.print("Konek ke: ");
  // Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Serial.print("MDNS: ");
  // Serial.println(MDNS);

  server.on("/", HOMEPAGE);
  server.on("/about", ABOUT);
  server.on("/on", onLED);
  server.on("/off", offLED);
  server.begin();
}

void loop() {
  MDNS.update();
  server.handleClient();
}
