#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>

//Network SSID
const char* ssid = "INTRUKTUR-TAV-2.4G";
const char* password = "skanebabisa1";

//pengenal host (server) = IP Address komputer server
const char* host = "siap.smknbansari.sch.id";
const int port = 80;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//Week Days
String weekDays[7] = { "Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jum'at", "Sabtu" };

//Month names
String months[12] = { "Januari", "Februari", "Maret", "April", "Mei", "Juni", "Juli", "Augustus", "September", "Oktober", "November", "Desember" };

#define LED_PIN D0  //D8
#define BUZ_PIN 5   //D1

//sediakan variabel untuk RFID
#define SDA_PIN 2  //D4
#define RST_PIN 0  //D3

String IDTAG;
String hasilTAG;
int i, j, berhasilBaca;
unsigned long timeout;
String dataMasuk, hasilData;
boolean parsing = false;
int loadInfo;
unsigned long previousMillis, currentMillis;
char chipID[25];

int ledState = LOW;

MFRC522 mfrc522(SDA_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);

  pinMode(BUZ_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  buzz(0);

  //setting koneksi wifi
  WiFi.hostname("NodeMCU");
  WiFi.begin(ssid, password);

  {
    //cek koneksi wifi
    Serial.println("Menyambungkan ke Wifi");
    Serial.println();
  }

  while (WiFi.status() != WL_CONNECTED) {
    //progress sedang mencari WiFi
    // blink(50);
    digitalWrite(LED_PIN, HIGH);
    Serial.print(".");
    // wait(200);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }

  // Initialize a NTPClient to get time
  timeClient.begin();
  int GMT = 7;
  timeClient.setTimeOffset(3600 * GMT);

  {
    Serial.println();
    Serial.println("Tersambung ke Wifi");
    Serial.println("IP Address : ");
    Serial.println(WiFi.localIP());
    Serial.println("MAC Address : ");
    Serial.println(WiFi.macAddress());
  }

  {
    int num = ESP.getChipId();
    itoa(num, chipID, 10);
    Serial.println("ID chip: ");
    Serial.println(chipID);
  }

  {
    SPI.begin();
    mfrc522.PCD_Init();
    Serial.println("Tempelkan Kartu RFID Anda ke Reader");
    Serial.println();
  }

  ntp();

  {
    buzz(3);
  }
}

void loop() {
  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("Menyambungkan lagi...");
    i = 0;
    return;
  }
  if (i == 0) {
    Serial.println("Koneksi OK");
    i = 1;
  }

  if (i == 1) {
    // blink(500);
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  berhasilBaca = bacaTag();

  if (berhasilBaca) {
    if (IDTAG) {
      buzz(1);
    } else {
      buzz(0);
    }

    if (hasilTAG != IDTAG) {
      hasilTAG = IDTAG;
      Serial.println("Nomor ID Kartu :" + IDTAG);
      String apiUrl = "/app/kirimkartu.php?nokartu=" + IDTAG;
      client.print(String("GET ") + apiUrl + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
    } else {
      Serial.println("...");
    }

    delay(1000);
  } else {
    hasilTAG = "";
  }

  buzz(0);
}

int bacaTag() {
  if (!mfrc522.PICC_IsNewCardPresent())
    return 0;

  if (!mfrc522.PICC_ReadCardSerial())
    return 0;

  IDTAG = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    IDTAG += mfrc522.uid.uidByte[i];
  }

  return 1;
}

void buzz(int loop) {
  if (loop == 0) {
    digitalWrite(BUZ_PIN, LOW);
  } else if (loop == 1) {
    digitalWrite(BUZ_PIN, HIGH);
  } else {
    for (int t = 0; t < loop; t++) {
      digitalWrite(BUZ_PIN, HIGH);
      delay(200);
      digitalWrite(BUZ_PIN, LOW);
      delay(200);
    }
  }
}

// void blink(int jeda) {
//   currentMillis = millis();

//   if (currentMillis - previousMillis >= jeda) {
//     previousMillis = currentMillis;
//     ledState = !ledState;
//     digitalWrite(LED_PIN, ledState);
//   }
// }

// void wait(unsigned long time) {
//   unsigned long start = millis();
//   while (millis() - start < time) {
//   };
// }

void ntp() {
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  //Get a time structure
  struct tm* ptm = gmtime((time_t*)&epochTime);
  String weekDay = weekDays[timeClient.getDay()];
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon + 1;

  String currentMonth_2digit;

  if (currentMonth < 10) {
    currentMonth_2digit = "0" + String(currentMonth);
  } else {
    currentMonth_2digit = String(currentMonth);
  }

  String currentMonthName = months[currentMonth - 1];
  int currentYear = ptm->tm_year + 1900;

  String hariTanggal = weekDay + ", " + monthDay + " " + currentMonthName + " " + currentYear;
  Serial.print("Current date: ");
  Serial.println(hariTanggal);

  String timestamp = String(currentYear) + "-" + currentMonth_2digit + "-" + String(monthDay) + " " + timeClient.getFormattedTime();
  Serial.print("timestamp: ");
  Serial.println(timestamp);

  Serial.println("");
}