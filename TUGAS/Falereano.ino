#include<NTPClient.h>
#include<ESP8266WiFi.h>
#include<WiFiUdp.h>
#include<LiquidCrystal_I2C.h>

//Setup WiFi
const char* ssid  ="INTRUKTUR-TAV-2.4G";
const char* password ="skanebabisa1";

//Set GMT+7
const long utcOffsetInSeconds=25200;

//Setting tanggal menjadi nama hari
char daysOfTheWeek[7][12]={"Minggu","Senin","selasa","Rabu","Kamis","Jum'at","Sabtu"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"id.pol.ntp.org",utcOffsetInSeconds);
LiquidCrystal_I2C lcd(0x3F,16,2);

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid,password);

  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

timeClient.begin();
lcd.begin(16,2);
lcd.init();
lcd.backlight();

lcd.setCursor(0,0);
lcd.print(">>TE Project>>");
lcd.setCursor(0,1);
lcd.print("JAM NTP-IoT>>");

delay(3000);
}

void loop() {
  timeClient.update();

  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(",");
  Serial.print("");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  Serial.println(timeClient.getFormattedTime());

  lcd.setCursor(0,0);
  lcd.print("JAM NTP-IoT>>");
  lcd.setCursor(0,1);
  lcd.print(daysOfTheWeek[timeClient.getDay()]);
  lcd.setCursor(6,1);
  lcd.print(", ");

  lcd.setCursor(8,1);
  lcd.print(timeClient.getFormattedTime());
}