#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include "index.h"

#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define RST_AP D5

ESP8266WebServer server(80);

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);  //Software I2C

const int pinLED = LED_BUILTIN;

void setup() {
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBIS08_tf);
  u8g2.drawStr(12, 10, "Mode Akses Point");
  u8g2.drawStr(12, 30, "SSID: bendzone");
  u8g2.drawStr(12, 50, "IP: 192.168.4.1");
  u8g2.sendBuffer();
  delay(1000);

  pinMode(pinLED, OUTPUT);
  pinMode(RST_AP, INPUT_PULLUP);
  digitalWrite(pinLED, HIGH);

  Serial.begin(115200);

  WiFiManager wifiManager;

  // reset wifi
  // if (digitalRead(RST_AP) == 0) {
  // wifiManager.resetSettings();
  // }

  wifiManager.autoConnect("bendzone");

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBIS08_tf);
  u8g2.drawStr(12, 30, "Nyambung Coy...");
  u8g2.sendBuffer();
  delay(1000);

  Serial.println("Nyambung Coy...");

  if (MDNS.begin("ben")) {
    Serial.println("MDNS responer started");
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_luBIS08_tf);
    u8g2.drawStr(12, 30, "MDNS responer started");
    u8g2.sendBuffer();
    delay(1000);
  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBIS08_tf);
  u8g2.drawStr(12, 30, "IP Address");
  u8g2.drawStr(0, 50, WiFi.localIP().toString().c_str());
  u8g2.sendBuffer();
  delay(1000);

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

// halaman webserver
void HOMEPAGE() {
  String page = FPSTR(MAIN_PAGE);
  server.send(200, "text/html", page);
}

void ABOUT() {
  server.send(200, "text/plain", "ABout");
}

void onLED() {
  digitalWrite(pinLED, LOW);
  String page = FPSTR(MAIN_PAGE);

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBIS14_tf);
  u8g2.drawStr(12, 30, "LED ON");
  u8g2.sendBuffer();

  server.send(200, "text/html", "ON");
}

void offLED() {
  digitalWrite(pinLED, HIGH);
  String page = FPSTR(MAIN_PAGE);

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBIS14_tf);
  u8g2.drawStr(12, 30, "LED OFF");
  u8g2.sendBuffer();

  server.send(200, "text/html", "OFF");
}

// halaman webserver
