#include <ESP8266WiFi.h>
#include <U8g2lib.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

const char* ssid = "INTRUKTUR-TAV-2.4G";
const char* password = "skanebabisa1";

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);

int GMT = 7;
int timezone = GMT * 3600;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

String namaHari[7] = { "Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jum'at", "Sabtu" };
String namaBulan[12] = { "Januari", "Februari", "Maret", "April", "Mei", "Juni", "Juli", "Agustus", "September", "Oktober", "November", "Desember" };

void setup() {
  u8g2.begin();

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBIS08_tf);
  u8g2.drawStr(0, 10, "Menyambungkan");
  u8g2.drawStr(0, 30, ssid);
  u8g2.sendBuffer();

  WiFi.begin(ssid, password);

  delay(2000);

  int dot = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    u8g2.drawStr(dot, 50, "...");
    u8g2.sendBuffer();
    dot = dot + 4;

    if (dot > 128)
      dot = 0;
  }

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBIS08_tf);
  u8g2.drawStr(0, 30, ssid);
  u8g2.drawStr(0, 50, "Tersambung");
  u8g2.sendBuffer();
  delay(1000);

  // Initialize a NTPClient to get time
  timeClient.begin();
  timeClient.setTimeOffset(timezone);

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBIS08_tf);
  u8g2.drawStr(0, 30, "JAM Siap");
  u8g2.sendBuffer();
  delay(1000);
}

void loop() {
  timeClient.update();

  time_t epochTime = timeClient.getEpochTime();

  struct tm* ptm = gmtime((time_t*)&epochTime);
  String hari = namaHari[timeClient.getDay()];
  int tanggalHari = ptm->tm_mday;
  int currentMonth = ptm->tm_mon + 1;

  String bulanSekarang = namaBulan[currentMonth - 1];
  int tahunSekarang = ptm->tm_year + 1900;
  int lastTwoDigits = tahunSekarang % 100;

  // Hari, Tanggal Bulan Tahun
  // Jum'at, 26 Januari 2024
  String Tanggal = String(tanggalHari) + " " + bulanSekarang + " " + tahunSekarang;

  String jam = timeClient.getFormattedTime();

  // tampilan
  u8g2.clearBuffer();
  // Tampilkan hari Jam
  u8g2.setFont(u8g2_font_luBIS14_tf);
  u8g2.drawStr(12, 30, jam.c_str());
  // Tampilkan hari tanggal
  u8g2.setFont(u8g2_font_luBIS08_tf);
  u8g2.drawStr(40, 50, hari.c_str());
  u8g2.drawStr(10, 60, Tanggal.c_str());
  u8g2.sendBuffer();
}
