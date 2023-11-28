#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "index.h"
#include "login.h"
#include "error.h"
#include <EEPROM.h>

#define buttonPin 2
const int eepromSize = 512;
const int debounceDelay = 50;

unsigned long buttonPressTime = 0;
unsigned long lastTimeConnected = 0;
boolean tombolDitekan = false;

String ssidNew = "", passNew, nodeviceNew, hostNew;
String usernameLogin, passwordLogin;

String chipID;

String formattedHtml = "Chip ID: %IDCHIP%";

char nodevice[20] = "";

AsyncWebServer server(80);

void writeStringToEEPROM(int startAddr, String data) {
  EEPROM.begin(eepromSize);
  for (int i = 0; i < data.length(); i++) {
    EEPROM.write(startAddr + i, data[i]);
  }
  EEPROM.write(startAddr + data.length(), '\0');
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
  return data.substring(0, data.length() - 1);
}

void bukaAP(String _text) {
  Serial.println("");
  Serial.println(_text);
  WiFi.disconnect();
  delay(1000);

  String ssidString = "SiAPP-Config32-" + chipID;
  const char *ssid = ssidString.c_str();
  const char *password = "siap$bos";

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  server.begin();
  IPAddress IP = WiFi.softAPIP();
  Serial.println("");
  Serial.print("AP: ");
  Serial.println(ssid);
  Serial.print("IP Address (AP): ");
  Serial.println(IP);
}

void handleReboot(AsyncWebServerRequest *request) {
  request->send_P(200, "text/html", selesai_html);
  delay(2000);
  ESP.restart();
}

void handleLogin(AsyncWebServerRequest *request) {
  boolean usernameOK = false;
  boolean passwordOK = false;

  usernameLogin = request->arg("username");
  passwordLogin = request->arg("password");

  if (usernameLogin == "siapconfig") {
    usernameOK = true;
  }

  if (passwordLogin == "siap$bos") {
    passwordOK = true;
  }

  if (usernameOK && passwordOK) {
    String formattedHtml = String(index_html);
    formattedHtml.replace("%IDCHIP%", chipID);
    formattedHtml.replace("%c", "/");
    formattedHtml.replace("%SSID_NEW%", ssidNew);
    formattedHtml.replace("%PASS_NEW%", passNew);
    formattedHtml.replace("%NODEVICE%", nodevice);
    formattedHtml.replace("%HOST%", hostNew);

    // Send the response
    request->send_P(200, "text/html", formattedHtml.c_str());
  } else if (!usernameOK && passwordOK) {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "Login Gagal - Username Login Salah!");
    formattedHtml.replace("%c", "/");
    request->send_P(404, "text/html", formattedHtml.c_str());
  } else if (usernameOK && !passwordOK) {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "Login Gagal - Password Login Salah!");
    formattedHtml.replace("%c", "/");
    request->send_P(404, "text/html", formattedHtml.c_str());
  } else if (usernameLogin == "" || passwordLogin == "") {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "Login Gagal - Login dulu ya..");
    formattedHtml.replace("%c", "/");
    request->send_P(404, "text/html", formattedHtml.c_str());
  } else {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "Login Gagal - Username dan Password Login Salah!");
    formattedHtml.replace("%c", "/");
    request->send_P(404, "text/html", formattedHtml.c_str());
  }
}

void handleForm(AsyncWebServerRequest *request) {
  ssidNew = request->arg("ssidNew");
  passNew = request->arg("passNew");
  nodeviceNew = request->arg("nodevice");
  hostNew = request->arg("host");

  Serial.println("");
  Serial.println("SSID: ");
  Serial.println(ssidNew);
  Serial.println("PASS: ");
  Serial.println(passNew);
  Serial.println("nodevice: ");
  Serial.println(nodeviceNew);
  Serial.println("HOST: ");
  Serial.println(hostNew);
  Serial.println("");

  writeStringToEEPROM(0, ssidNew);
  writeStringToEEPROM(64, passNew);
  writeStringToEEPROM(128, nodeviceNew);
  writeStringToEEPROM(192, hostNew);


  // jangan menyimpan config kosong
  if (ssidNew == "" && hostNew == "" && nodeviceNew == "") {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "GAGAL melakukan Konfigurasi");
    formattedHtml.replace("%c", "/setting");
    request->send_P(404, "text/html", formattedHtml.c_str());
  } else {
    // Save values to EEPROM
    writeStringToEEPROM(0, ssidNew);
    writeStringToEEPROM(64, passNew);
    writeStringToEEPROM(128, nodeviceNew);
    writeStringToEEPROM(192, hostNew);


    String formattedHtml = String(sukses_html);
    formattedHtml.replace("%IDCHIP%", chipID);
    formattedHtml.replace("%SSID_NEW%", ssidNew);
    formattedHtml.replace("%PASS_NEW%", passNew);
    formattedHtml.replace("%NODEVICE%", nodeviceNew);
    formattedHtml.replace("%HOST%", hostNew);
    request->send_P(200, "text/html", formattedHtml.c_str());

    delay(2000);

    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssidNew.c_str(), passNew.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
      delay(1000);
      Serial.print(".");
      attempts++;
    }

    if (WiFi.status() != WL_CONNECTED) {
      // Jika tidak terkoneksi dalam 30 detik, pindah ke mode Akses Poin
      bukaAP("Tidak dapat terhubung ke WiFi. Memulai mode Akses Poin...");
    } else {
      Serial.println("");
      Serial.println("Terhubung Ke Jaringan");
    }
  }
}

void checkButton() {
  int buttonPressed = digitalRead(buttonPin);

  if (tombolDitekan == false) {
    buttonPressTime = millis();
  }

  if (buttonPressed == LOW) {
    tombolDitekan = true;
    digitalWrite(0, HIGH);
    delay(100);
    digitalWrite(0, LOW);

    Serial.print("#");
    if (millis() - buttonPressTime > 5000) {
      tombolDitekan = false;
      bukaAP("Tombol telah ditekan selama 5 detik. Memulai mode Akses Poin...");
    }
  } else {
    tombolDitekan = false;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  chipID = idChip();

  ssidNew = readStringFromEEPROM(0);
  passNew = readStringFromEEPROM(64);
  nodeviceNew = readStringFromEEPROM(128);
  hostNew = readStringFromEEPROM(192);

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
      nodeviceNew = readStringFromEEPROM(128);
      hostNew = readStringFromEEPROM(192);

      strcpy(nodevice, nodeviceNew.length() > 0 ? nodeviceNew.c_str() : nodevice);

      Serial.print("SSID: ");
      Serial.println(ssidNew);
      Serial.print("pass: ");
      Serial.println(passNew);
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

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(0, OUTPUT);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String formattedHtml = String(login_html);
    formattedHtml.replace("%IDCHIP%", chipID);
    formattedHtml.replace("%c", "/");
    request->send_P(200, "text/html", formattedHtml.c_str());
  });

  server.on("/action_page", HTTP_POST, handleForm);
  server.on("/reboot", HTTP_GET, handleReboot);
  server.on("/setting", HTTP_POST, handleLogin);

  server.onNotFound([](AsyncWebServerRequest *request) {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "404 - Halaman Tidak ditemukan");
    formattedHtml.replace("%c", "/");
    request->send_P(404, "text/html", error_html);
  });

  delay(2000);

  server.begin();

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  Serial.print("Chip ID: ");
  Serial.println(chipID);
}

void loop() {
  checkButton();

  if (WiFi.status() == WL_CONNECTED) {
    // Jalankan logika utama di sini ketika terhubung ke WiFi
    // Serial.print("+");
    // ... (Tambahkan logika utama Anda di sini)

    // Baca tag RFID
    if (bacaTag()) {
      Serial.print("ID Tag: ");
      Serial.println(IDTAG);
    }

    lastTimeConnected = millis();
  } else {
    if (millis() - lastTimeConnected >= 30000) {
      bukaAP("30 detik tidak dapat menyambung WiFi, buka mode AP");
    }
  }
}

String idChip() {
  String id, buf_id;
  uint64_t chipid;
  char ssid[13];
  chipid = ESP.getEfuseMac();  //The chip ID is essentially its MAC address(length: 6 bytes).
  uint16_t chip = (uint16_t)(chipid >> 32);
  snprintf(ssid, 13, "%04X%08X", chip, (uint32_t)chipid);
  for (int i = 0; i < 12; i++) {
    buf_id += String(ssid[i]);
  }
  id = buf_id;
  buf_id = "";
  // Serial.println(id);

  return id;
}

int bacaTag() {
  if (!mfrc522.PICC_IsNewCardPresent())
    return 0;

  if (!mfrc522.PICC_ReadCardSerial())
    return 0;

  memset(IDTAG, 0, sizeof(IDTAG));
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    snprintf(IDTAG, sizeof(IDTAG), "%s%02X", IDTAG, mfrc522.uid.uidByte[i]);
  }

  return 1;
}