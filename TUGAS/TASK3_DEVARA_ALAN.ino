#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <LiquidCrystal_I2C.h>

const char* ssid      = "INTRUKTUR-TAV-2.4G";
const char* password  = "skanebabisa1";
const long utc0ffsetInSeconds = 25200;
char days0fTheWeek[7][12] = {"MINGGU","SENIN","SELASA","RABU","KAMIS","JUM'AT","SABTU"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"id.pool.ntp.org",utc0ffsetInSeconds);
LiquidCrystal_I2C lcd(0x27,16,2);
void setup() {
  // put your setup code here, to run once:
 Serial.begin(115200);
 WiFi.begin(ssid, password);
 while(WiFi.status() != WL_CONNECTED){
 delay ( 500 );
 Serial.print(".");
 }

 timeClient.begin();
 lcd.begin(16,2);
 lcd.init();
 lcd.backlight();
 
 lcd.setCursor(0,0);
 lcd.print(">> TE Project >>");
 lcd.setCursor(0,1);
 lcd.print("JAM NTP - IoI >>");

 delay(3000);
}
 
void loop() {
  // put your main code here, to run repeatedly:
  timeClient.update(); 
  
  Serial.print(days0fTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print("");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  Serial.println(timeClient.getFormattedTime());

 lcd.setCursor(0,0);
 lcd.print(">> JAM NTP_IoI >>");
 lcd.setCursor(0,1);
 lcd.print(days0fTheWeek[timeClient.getDay()]);

 lcd.setCursor(6,1);
 lcd.print(", ");
 lcd.setCursor(8,1);
 lcd.print(timeClient.getFormattedTime());

 
}
