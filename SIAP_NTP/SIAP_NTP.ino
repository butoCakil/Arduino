#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Replace with your network credentials
const char *ssid = "INTRUKTUR-TAV-2.4G";
const char *password = "skanebabisa1";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//Week Days
String weekDays[7] = { "Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jum'at", "Sabtu" };

//Month names
String months[12] = { "Januari", "Februari", "Maret", "April", "Mei", "Juni", "Juli", "Augustus", "September", "Oktober", "November", "Desember" };

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.print("Menyambunkan ke ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  int GMT = 7;
  timeClient.setTimeOffset(3600 * GMT);
}

void loop() {
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  //Get a time structure
  struct tm *ptm = gmtime((time_t *)&epochTime);
  String weekDay = weekDays[timeClient.getDay()];
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon + 1;
  String currentMonthName = months[currentMonth - 1];
  int currentYear = ptm->tm_year + 1900;
  

  String hariTanggal = weekDay + ", " + monthDay + " " + currentMonthName + " " + currentYear;
  Serial.print("Current date: ");
  Serial.println(hariTanggal);

  String timestamp = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay) + " " + timeClient.getFormattedTime();
  Serial.print("timestamp: ");
  Serial.println(timestamp);

  Serial.println("");

  delay(2000);
}