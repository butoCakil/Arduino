#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <LiquidCrystal_I2C.h>

// Setup Wifi
const char *ssid     = "UPTOME";
const char *password = "kurniasandi";


const long utcOffsetInSeconds = 25200;

// Setting tanggal menjadi nama hari
char daysOfTheWeek[7][12] = {"Minggu ", "Senin  ", "Selasa ", "Rabu   ", "Kamis  ", "Jum'at ", "Sabtu  "};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "id.pool.ntp.org", utcOffsetInSeconds);
LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup(){
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();
  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print(">> ESE Project >");
  lcd.setCursor(0, 1);      
  lcd.print("JAM NTP - IoT >>");

  delay(3000);
}

void loop() {
  timeClient.update();

  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print("");
  Serial.print(timeClient.getHours());  
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  Serial.println(timeClient.getFormattedTime());
  
  lcd.setCursor(0, 0);
  lcd.print("JAM NTP - IoT >>");
  lcd.setCursor(0, 1);      
  lcd.print(daysOfTheWeek[timeClient.getDay()]);
  lcd.setCursor(6, 1);      
  lcd.print(", ");

//  lcd.setCursor(8, 1);      
//  lcd.print(timeClient.getHours());
//  lcd.setCursor(10, 1);      
//  lcd.print(":");
//
//  if(timeClient.getMinutes() < 10){
//    lcd.setCursor(11, 1);      
//    lcd.print("0");
//    lcd.setCursor(12, 1);      
//    lcd.print(timeClient.getMinutes());
//  } else {
//    lcd.setCursor(11, 1);      
//    lcd.print(timeClient.getMinutes());
//  }
//  
//  lcd.setCursor(13, 1);      
//  lcd.print(":");
//
//  if(timeClient.getSeconds() < 10){
//    lcd.setCursor(14, 1);      
//    lcd.print("0");
//    lcd.setCursor(15, 1);      
//    lcd.print(timeClient.getSeconds());
//  } else {
//    lcd.setCursor(14, 1);      
//    lcd.print(timeClient.getSeconds());
//  }

  lcd.setCursor(8, 1);      
  lcd.print(timeClient.getFormattedTime());
  
  delay(1000);
}
