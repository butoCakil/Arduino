#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "HTML.h"
#include <EEPROM.h>

#define buttonPin D8  // Pin tempat tombol push button dihubungkan
const int eepromSize = 512;
const int debounceDelay = 50;  // Waktu debounce dalam milidetik

unsigned long buttonPressTime = 0;
boolean tombolDitekan = false;

const char *ssid = "Mencari Korban Hack";
const char *password = "";

String ssidNew = "", passNew, hotspotNew, nodeviceNew, hostNew;

ESP8266WebServer server(80);

void writeStringToEEPROM(int startAddr, String data) {
  EEPROM.begin(eepromSize);
  for (int i = 0; i < data.length(); i++) {
    EEPROM.write(startAddr + i, data[i]);
  }
  EEPROM.write(startAddr + data.length(), '\0');  // Null-terminate the string
  EEPROM.commit();
  EEPROM.end();
}

String readStringFromEEPROM(int startAddr) {
  EEPROM.begin(eepromSize);
  String data = "";
  char ch;
  do {
    ch = EEPROM.read(startAddr + data.length());
    data += ch;
  } while (ch != '\0');
  EEPROM.end();
  return data.substring(0, data.length() - 1);  // Remove the null character
}

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleNotFound() {
  server.send(404, "text/plain", "404 Not Found");
}

void handleForm() {
  ssidNew = server.arg("ssidNew");
  passNew = server.arg("passNew");
  hotspotNew = server.arg("hotspot");
  nodeviceNew = server.arg("nodevice");
  hostNew = server.arg("host");

  Serial.println("");
  Serial.println("SSID: ");
  Serial.println(ssidNew);
  Serial.println("PASS: ");
  Serial.println(passNew);
  Serial.println("HOTSPOT: ");
  Serial.println(hotspotNew);
  Serial.println("nodevice: ");
  Serial.println(nodeviceNew);
  Serial.println("HOST: ");
  Serial.println(hostNew);
  Serial.println("");

  // Save values to EEPROM
  writeStringToEEPROM(0, ssidNew);
  writeStringToEEPROM(64, passNew);
  writeStringToEEPROM(128, hotspotNew);
  writeStringToEEPROM(192, nodeviceNew);
  writeStringToEEPROM(256, hostNew);

  server.send(200, "text/html", sukses_html);
  delay(2000);  // Agar perangkat dapat mengirimkan data sebelum disconnect

  WiFi.softAPdisconnect(true);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssidNew.c_str(), passNew.c_str());

  // Tunggu sampai terhubung
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    // Jika tidak terkoneksi dalam 30 detik, pindah ke mode Akses Poin

  } else {
    Serial.println("");
    Serial.println("Terhubung Ke Jaringan");
  }
}

void checkButton() {
  int buttonPressed = digitalRead(buttonPin);

  if (tombolDitekan == false) {
    buttonPressTime = millis();
  }

  if (buttonPressed == HIGH) {
    tombolDitekan = true;
    digitalWrite(D0, HIGH);
    delay(100);
    digitalWrite(D0, LOW);

    Serial.println("Tombol Ditekan");
    Serial.print("Tunggu 5 Detik: ");
    // Tombol dilepas, cek apakah sudah lebih dari 5 detik
    if (millis() - buttonPressTime > 5000) {
      tombolDitekan = false;

      // Tombol ditekan selama 5 detik atau lebih, pindah ke mode Akses Poin
      bukaAP("Tombol telah ditekan selama 5 detik. Memulai mode Akses Poin...");
    }
  } else {
    tombolDitekan = false;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  ssidNew = readStringFromEEPROM(0);
  passNew = readStringFromEEPROM(64);
  hotspotNew = readStringFromEEPROM(128);
  nodeviceNew = readStringFromEEPROM(192);
  hostNew = readStringFromEEPROM(256);

  // Jika ada nilai SSID dan password di EEPROM, coba terhubung ke WiFi
  if (ssidNew != "" && passNew != "") {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssidNew.c_str(), passNew.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
      delay(1000);
      Serial.print(".");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("");
      Serial.println("Terhubung Ke Jaringan");

      Serial.println("Connected to WiFi. Running main logic...");
      ssidNew = readStringFromEEPROM(0);
      passNew = readStringFromEEPROM(64);
      hotspotNew = readStringFromEEPROM(128);
      nodeviceNew = readStringFromEEPROM(192);
      hostNew = readStringFromEEPROM(256);

      Serial.print("SSID: ");
      Serial.println(ssidNew);
      Serial.print("pass: ");
      Serial.println(passNew);
      Serial.print("hotspot: ");
      Serial.println(hotspotNew);
      Serial.print("nodevice: ");
      Serial.println(nodeviceNew);
      Serial.print("host: ");
      Serial.println(hostNew);
      Serial.println();
    } else {
      bukaAP("Tidak dapat terhubung ke WiFi. Memulai mode Akses Poin...");
    }
  } else {
    bukaAP("SSID dan password tidak ditemukan di EEPROM. Memulai mode Akses Poin...");
  }

  pinMode(buttonPin, INPUT);
  pinMode(D0, OUTPUT);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/action_page", handleForm);
  server.onNotFound(handleNotFound);

  delay(2000);
}

void loop() {
  checkButton();  // Periksa status tombol push button

  if (WiFi.status() == WL_CONNECTED) {
    // Jalankan logika utama di sini ketika terhubung ke WiFi
    // Serial.print("+");
    // ... (Tambahkan logika utama Anda di sini)
  } else {
    // Logika Akses Poin
    server.handleClient();
  }
}

void bukaAP(String _text) {
  Serial.println("");
  Serial.println(_text);
  WiFi.disconnect();
  // delay sebelum memulai konfigurasi AP
  delay(1000);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  server.begin();  // Mulai server dalam mode Akses Poin

  IPAddress IP = WiFi.softAPIP();
  Serial.println("");
  Serial.print("AP: ");
  Serial.println(ssid);
  Serial.print("IP Address (AP): ");
  Serial.println(IP);
}
