#include <Arduino.h>
#include <TM1637Display.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <EEPROM.h>

// Setup Wifi
// const char *ssid = "ASSEMBLY ONLY";
// const char *password = "onlyassemblytebos";

const char *ssid = "IOT";
const char *password = "microkontroler";
WiFiServer server(80);

boolean timerAktif = true;
boolean timerBerhenti = false;
boolean dot = false;
boolean relayState = false;
unsigned long waktuSebelum = 0;
unsigned long waktuDetikSebelum = 0;

int runDetik = 15;
int runMenit = 1;
int runJam = 0;

// Deklarasi NTP
// Define NTP Client to get time ((GMT+7) = 7 * 60 * 60)
const long utcOffsetInSeconds = 25200;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "id.pool.ntp.org", utcOffsetInSeconds);

// Module connection pins (Digital Pins)
// const int buzzerPin = 3;  // Ganti dengan pin yang sesuai
#define CLK 2
#define DIO 0
// #define RELAY 1

// The amount of time (in milliseconds) between tests
#define TEST_DELAY 2000
#define SATU_DETIK 1000
#define STGH_DETIK 500
#define FLASH 100

const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,          // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
  SEG_C | SEG_E | SEG_G,                          // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G           // E
};

const uint8_t SEG_STRT[] = {
  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,  // S
  SEG_F | SEG_G | SEG_E | SEG_D,          // t
  SEG_E | SEG_G,                          // r
  SEG_F | SEG_G | SEG_E | SEG_D,          // t
};

const uint8_t SEG_STOP[] = {
  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,          // S
  SEG_F | SEG_G | SEG_E | SEG_D,                  // t
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
  SEG_A | SEG_B | SEG_F | SEG_G | SEG_E,          // t
};

TM1637Display display(CLK, DIO);

void setup() {
  pinMode(CLK, OUTPUT);
  pinMode(DIO, OUTPUT);
  // pinMode(RELAY, FUNCTION_0);
  // pinMode(buzzerPin, FUNCTION_0);

  // digitalWrite(RELAY, LOW);
  // digitalWrite(buzzerPin, LOW);

  // beepBuzzer(1, 100, 500);

  Serial.begin(115200);
  display.setBrightness(0x0f);

  // Start!
  // Run through all the dots
  for (int k = 0; k <= 4; k++) {
    display.showNumberDecEx(0, (0x80 >> k), true);
    delay(SATU_DETIK);
  }

  // Konek ke Wi-Fi
  Serial.print("Menyambungkan ke  ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    blinkDot(0, SATU_DETIK, false);
  }

  // beepBuzzer(1, 100, 500);

  Serial.println();
  Serial.print("Tersambung: ");
  Serial.println(ssid);

  // Sinkronisasi NTP
  timeClient.begin();

  // Done!
  display.setSegments(SEG_DONE);

  int jam = timeClient.getHours();
  int menit = timeClient.getMinutes();

  // Serial.print("JAM: ");
  // Serial.print(jam);
  // Serial.print(", MENIT: ");
  // Serial.println(menit);

  // beepBuzzer(2, 100, 500);

  // Start!
  display.setSegments(SEG_STRT);

  server.begin();

  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  delay(3000);
}

void loop() {
  serverSet();

  if (timerAktif) {
    runTimer();
  } else {
    tampilJam();
  }
}

void runTimer() {
  // ambil waktu dari `runJam`, `runMenit`, dan `runDetik`
  int digitAwal, digitAkhir;

  if (runJam > 0) {
    // tampil jam dan menit;
    if (runDetik <= 0) {
      runDetik = 59;
      runMenit--;
    }

    if (runMenit < 0) {
      runMenit = 59;
      runJam--;
    }

    digitAkhir = runMenit;
    digitAwal = runJam;
  } else {
    if (runJam <= 0 && runDetik <= 0 && runMenit <= 0) {

      if (timerBerhenti == false) {
        // beepBuzzer(3, 100, 500);
      }

      timerBerhenti = true;

      runDetik = 0;
      runMenit = 0;

      // Serial.println("STOP");
    } else {
      // tampil menit dan detik;
      if (runDetik <= 0) {
        runDetik = 59;
        runMenit--;
      }
    }

    digitAkhir = runDetik;
    digitAwal = runMenit;
  }

  if (timerBerhenti == false) {
    tampilTimer(digitAwal, digitAkhir);

    unsigned long waktuDetikSekarang = millis();

    if (waktuDetikSekarang - waktuDetikSebelum >= SATU_DETIK) {
      runDetik--;
      waktuDetikSebelum = millis();
    }
  } else {
    // Stop!
    display.setSegments(SEG_STOP);
  }
}

void tampilTimer(int lamaJam, int lamaMenit) {
  unsigned long jamTimer = (lamaJam * 100) + lamaMenit;

  blinkDot(jamTimer, FLASH, true);
}

void tampilJam() {
  timeClient.update();

  int jam = timeClient.getHours();
  int menit = timeClient.getMinutes();
  long jamSekarang = (jam * 100) + menit;

  blinkDot(jamSekarang, STGH_DETIK, true);
}

void blinkDot(int getTime, int _TUNDA, boolean status) {
  unsigned long waktuSekarang = millis();

  if (waktuSekarang - waktuSebelum >= _TUNDA) {
    if (dot == true) {
      dot = false;
      display.showNumberDecEx(getTime, 0b11000000, status);
    } else {
      dot = true;
      display.showNumberDecEx(getTime, 0b00000000, status);
    }

    waktuSebelum = millis();
  }
}

// void beepBuzzer(int jumlahBeep, int durasiBeep, int jedaAntarBeep) {
//   unsigned long waktuAwal = millis();

//   for (int i = 0; i < jumlahBeep; i++) {
//     digitalWrite(buzzerPin, LOW);  // Matikan buzzer setelah durasi beep
//     delay(durasiBeep);
//     digitalWrite(buzzerPin, HIGH);  // Nyalakan buzzer
//     delay(jedaAntarBeep);
//   }

//   delay(jedaAntarBeep);

//   digitalWrite(buzzerPin, LOW);  // Pastikan buzzer dimatikan setelah loop selesai
// }
