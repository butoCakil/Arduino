#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <MFRC522.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#include <Arduino.h>
#include <U8g2lib.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include "index.h"
#include "login.h"
#include "error.h"

// Ganti true jika berhotspot tanpa password
boolean modeHotspot = false;

// MQTT Broker Configuration
char nodevice[20] = "2309G005";  // GERBANG / PRESENSI MASUK (max 20 characters)
// char nodevice[20] = "2309MAS005";  // PEMBIASAAN MASJID (max 20 characters)
// char nodevice[20] = "2310IZ002";  // POS SATPAM (IJIN) (max 20 characters)
// char nodevice[20] = "2309NA003";  // PEMBIASAAN MASJID (max 20 characters)

char* mqtt_server = "172.16.80.123";  // Ganti dengan alamat IP broker MQTT Anda
// const char* mqtt_server = "10.16.0.102";  // Ganti dengan alamat IP broker MQTT Anda

const int mqtt_port = 1883;          // Port MQTT default
const char* mqtt_user = "ben";       // Username MQTT Anda
const char* mqtt_password = "1234";  // Password MQTT Anda

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);  //Software I2C

boolean aktifSerialMsg = false;
boolean autoRestart = false;
boolean tungguRespon = false;
boolean saatnyaRestart = false;
boolean modeAPaktif = false;

#define BUZ_PIN D0  // D0 - MERAH
#define SET_BTN D8  // Push BUtton SET

// RFID
#define SDA_PIN 2  // D4
#define RST_PIN 0  // D3

char IDTAG[20];
char chipID[25];                       // Store ESP8266 Chip ID
char key[50] = "1234567890987654321";  // Change to your desired Key (max 20 characters)

String receivedMessage = "";

MFRC522 mfrc522(SDA_PIN, RST_PIN);

WiFiClient espClient;
PubSubClient client(espClient);

// Variabel untuk memantau waktu terakhir pembacaan kartu RFID
unsigned long lastRFIDReadTime = 0;
const unsigned long RFID_READ_INTERVAL = 600000;  // 10 menit dalam milidetik (10 * 60 * 1000)

unsigned long lastTunggurespon = 0;
const unsigned long TUNGGU_RESPON_SERVER = 5000;

// Waktu mulai BootLoad
unsigned long startTimeBootLoad;

int nom;

int screenWidth = u8g2.getWidth();
int screenHeight = u8g2.getHeight();

// EEPROM Konfigurasi
const int eepromSize = 512;
const int debounceDelay = 50;  // Waktu debounce dalam milidetik

unsigned long buttonPressTime = 0;
boolean tombolDitekan = false;

const char* ssid = "SiAPP-Config";
const char* password = "siap$bos";

String ssidNew = "", passNew, nodeviceNew, hostNew;
String usernameLogin, passwordLogin;

ESP8266WebServer server(80);

#include "function.h"

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
  String formattedHtml = String(login_html);
  formattedHtml.replace("%s", chipID);
  formattedHtml.replace("%c", "/");
  server.send(200, "text/html", formattedHtml);
}

void handleNotFound() {
  String formattedHtml = String(error_html);
  formattedHtml.replace("%s", "404 - Halaman Tidak ditemukan");
  formattedHtml.replace("%c", "/");
  server.send(404, "text/html", formattedHtml);
}

void handelLogin() {
  boolean usernameOK = false;
  boolean passwordOK = false;

  usernameLogin = server.arg("username");
  passwordLogin = server.arg("password");

  if (usernameLogin == "siapconfig") {
    usernameOK = true;
  }

  if (passwordLogin == "siap$bos") {
    passwordOK = true;
  }

  if (usernameOK && passwordOK) {
    String formattedHtml = String(index_html);
    formattedHtml.replace("%s", chipID);
    formattedHtml.replace("%c", "/");
    formattedHtml.replace("%SSID_NEW%", ssidNew);
    formattedHtml.replace("%PASS_NEW%", passNew);
    formattedHtml.replace("%NODEVICE%", nodevice);
    formattedHtml.replace("%HOST%", mqtt_server);

    // Send the response
    server.send(200, "text/html", formattedHtml);
  } else if (!usernameOK && passwordOK) {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "Login Gagal - Username Login Salah!");
    formattedHtml.replace("%c", "/");
    server.send(404, "text/html", formattedHtml);
  } else if (usernameOK && !passwordOK) {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "Login Gagal - Password Login Salah!");
    formattedHtml.replace("%c", "/");
    server.send(404, "text/html", formattedHtml);
  } else if (usernameLogin == "" || passwordLogin == "") {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "Login Gagal - Login dulu ya..");
    formattedHtml.replace("%c", "/");
    server.send(404, "text/html", formattedHtml);
  } else {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "Login Gagal - Username dan Password Login Salah!");
    formattedHtml.replace("%c", "/");
    server.send(404, "text/html", formattedHtml);
  }
}

void handleReboot() {
  server.send(200, "text/html", selesai_html);
  buzzBasedOnMessage("400");
  boot("Restart dalam 3 detik");
  delay(2000);
  ESP.restart();
}

void handleForm() {
  ssidNew = server.arg("ssidNew");
  passNew = server.arg("passNew");
  nodeviceNew = server.arg("nodevice");
  hostNew = server.arg("host");

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

  // jangan menyimpan config kosong
  if (ssidNew == "" && hostNew == "" && nodeviceNew == "") {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "GAGAL melakukan Konfigurasi");
    formattedHtml.replace("%c", "/setting");
    server.send(404, "text/html", formattedHtml);
  } else {
    // Save values to EEPROM
    writeStringToEEPROM(0, ssidNew);
    writeStringToEEPROM(64, passNew);
    writeStringToEEPROM(192, nodeviceNew);
    writeStringToEEPROM(256, hostNew);


    String formattedHtml = String(sukses_html);
    formattedHtml.replace("%s", chipID);
    formattedHtml.replace("%SSID_NEW%", ssidNew);
    formattedHtml.replace("%PASS_NEW%", passNew);
    formattedHtml.replace("%NODEVICE%", nodeviceNew);
    formattedHtml.replace("%HOST%", hostNew);
    server.send(200, "text/html", formattedHtml);
    startTimeBootLoad = millis();
    buzz(2);
    bootLoad("Menyimpan Config..");
    delay(2000);  // Agar perangkat dapat mengirimkan data sebelum disconnect

    WiFi.softAPdisconnect(true);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssidNew.c_str(), passNew.c_str());

    // Tunggu sampai terhubung
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
      delay(1000);
      Serial.print("i");
      attempts++;
    }

    if (WiFi.status() != WL_CONNECTED) {
      // Jika tidak terkoneksi dalam 30 detik,
      // tampilkan wifi tidak dapat terhubung
      buzz(3);
      modeAPaktif = true;
      displayIconStatusText(ssid, "WiFi Gagal Konek.. RESET / Set AP", epd_bitmap_x_3x);
      delay(1000);
    } else {
      buzz(2);
      modeAPaktif = false;
      Serial.println("");
      Serial.println("Terhubung Ke Jaringan");
      displayIconStatusText(ssid, "berhasil konek ke WiFi", epd_bitmap_check_3x);

      delay(2000);
      boot("Restart dalam 3 detik");
      delay(2000);
      buzzBasedOnMessage("400");
      ESP.restart();
    }
  }
}

void checkButton() {
  int buttonPressed = digitalRead(SET_BTN);

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
      modeAPaktif = true;
      tombolDitekan = false;

      startTimeBootLoad = millis();
      buzz(1);
      bootLoad("Memulai Mode AP");
      delay(1000);
      // Tombol ditekan selama 5 detik atau lebih, pindah ke mode Akses Poin
      bukaAP("Memulai mode Akses Poin...");
    }
  } else {
    tombolDitekan = false;
  }
}

void setup() {
  u8g2.begin();
  Serial.begin(115200);
  Serial.println();
  Serial.println("Start");
  delay(1000);

  // EEPROM config
  ssidNew = readStringFromEEPROM(0);
  passNew = readStringFromEEPROM(64);

  // Set bus speed to 400 kHz
  u8g2.setBusClock(400000);
  u8g2.setContrast(255);  // Set contrast to maximum

  u8g2.clearBuffer();                  // clear the internal memory
  u8g2.setFont(u8g2_font_luBIS08_tf);  // choose a suitable font
  drawWrappedText("SIAPP", screenWidth / 2, 10, screenWidth, u8g2_font_luBIS08_tf);
  u8g2.sendBuffer();

  boot("booting...");

  // Get ESP8266 Chip ID
  int num = ESP.getChipId();
  itoa(num, chipID, 10);

  // Congig WiFi
  // Jika ada nilai SSID dan password di EEPROM, coba terhubung ke WiFi
  if (ssidNew != "" && passNew != "") {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssidNew.c_str(), passNew.c_str());

    int attempts = 0;
    startTimeBootLoad = millis();
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
      delay(1000);
      Serial.print(".");
      bootLoad("Menyambung ke WiFi");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      modeAPaktif = false;
      Serial.println("");
      Serial.println("Terhubung Ke Jaringan");

      ssidNew = readStringFromEEPROM(0);
      passNew = readStringFromEEPROM(64);
      nodeviceNew = readStringFromEEPROM(192);
      hostNew = readStringFromEEPROM(256);

      strcpy(nodevice, nodeviceNew.length() > 0 ? nodeviceNew.c_str() : nodevice);
      strcpy(mqtt_server, hostNew.length() > 0 ? hostNew.c_str() : mqtt_server);

      Serial.print("SSID: ");
      Serial.println(ssidNew);
      Serial.print("pass: ");
      Serial.println(passNew);
      Serial.print("nodevice: ");
      Serial.println(nodeviceNew);
      Serial.print("host: ");
      Serial.println(hostNew);
      Serial.println();

      displayIconStatusText(ssid, "Tersambung ke WiFi", epd_bitmap_check_3x);

      Serial.println();
      Serial.println("Tersambung ke WiFi");
      Serial.println();

      Serial.println("IP Address: ");
      Serial.println(WiFi.localIP());
      Serial.println("MAC Address: ");
      Serial.println(WiFi.macAddress());

      // tampilkan idchip
      Serial.println("Chip ID: ");
      Serial.println(chipID);

      delay(1000);

      // Setup MQTT client
      client.setServer(mqtt_server, mqtt_port);
      client.setCallback(callback);

      SPI.begin();
      mfrc522.PCD_Init();

      startTimeBootLoad = millis();
      while (!client.connected()) {
        bootLoad("Menyambungkan ke Server..");
        if (client.connect("NodeMCUClient", mqtt_user, mqtt_password)) {
          modeAPaktif = false;
          if (aktifSerialMsg)
            Serial.println("Tersambung ke MQTT Broker");

          buzzBasedOnMessage("200");
        } else {
          modeAPaktif = true;
          Serial.println("Koneksi MQTT gagal. Mengulangi koneksi...");

          displayIconStatusText("SIAPP: SERVER", "Gagal konek Server!", epd_bitmap_x_3x);
        }
      }

      // Subscribe to a topic
      String topic = "responServer_";
      topic += nodevice;
      client.subscribe(topic.c_str(), 0);

      displayIconStatusText(nodevice, "Tersambung ke Server", epd_bitmap_check_3x);

      // Initialize a NTPClient to get time
      timeClient.begin();
      int GMT = 7;
      timeClient.setTimeOffset(3600 * GMT);

      buzz(2);

      Serial.println("Tempelkan kartu RFID..");
    } else {
      modeAPaktif = true;
      displayIconStatusText(ssid, "WiFi Gagal Konek.. RESET / Set AP", epd_bitmap_x_3x);
      delay(1000);
    }
  } else {
    modeAPaktif = true;
    displayIconStatusText(ssid, "SSID Kosong! Mulai AP..", epd_bitmap_x_3x);
    delay(1000);
    bukaAP("SSID dan password tidak ditemukan di EEPROM. Memulai mode Akses Poin...");
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/setting", handelLogin);
  server.on("/action_page", HTTP_POST, handleForm);
  server.on("/reboot", handleReboot);
  server.onNotFound(handleNotFound);

  // pinMode(LED_PIN, OUTPUT);
  pinMode(BUZ_PIN, OUTPUT);
  // pinMode(OKE_PIN, OUTPUT);
  pinMode(SET_BTN, INPUT);

  client.disconnect();
  buzz(2);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    server.handleClient();
  }

  checkButton();
  int berhasilBaca = bacaTag();

  client.loop();

  if (modeAPaktif == false) {
    homeLCD();
  }

  if (berhasilBaca) {
    if (autoRestart) {
      lastRFIDReadTime = millis();  // Perbarui waktu terakhir pembacaan kartu RFID
    }

    static char hasilTAG[20] = "";  // Store previous tag ID

    if (strcmp(hasilTAG, IDTAG) != 0) {
      buzz(1);
      noLoadBarJustText("Membaca ID Kartu");
      reconnect();

      strcpy(hasilTAG, IDTAG);

      if (aktifSerialMsg)
        Serial.println("ID Kartu: " + String(IDTAG));

      // Send data to server and receive JSON response
      String jsonResponse = sendCardIdToServer(IDTAG);

      tungguRespon = true;
      lastTunggurespon = millis();

      if (aktifSerialMsg)
        Serial.println("Selesai kirim untuk ID: " + String(IDTAG));

    } else {
      if (aktifSerialMsg)
        Serial.println("-");
    }

    delay(900);
    strcpy(hasilTAG, "");
  } else {
    strcpy(IDTAG, "");
  }

  buzz(0);
  receivedMessage = "";
  if (modeAPaktif == false) {
    u8g2.drawXBM(112, 0, 16, 16, epd_bitmap_loop_circular_2x_120);
    u8g2.sendBuffer();
  }

  // Periksa apakah sudah waktunya untuk restart
  if (autoRestart) {
    unsigned long currentTime = millis();
    if (currentTime - lastRFIDReadTime > RFID_READ_INTERVAL) {
      saatnyaRestart = true;
    }

    if (saatnyaRestart) {
      if (aktifSerialMsg)
        Serial.println("Tidak ada aktifitas pembacaan kartu RFID selama 10 menit. Melakukan restart...");

      boot("Restart dalam 3 detik");
      buzzBasedOnMessage("400");
      delay(1000);
      ESP.restart();
    }
  }

  if (tungguRespon) {
    unsigned long currentTime2 = millis();
    if (currentTime2 - lastTunggurespon > TUNGGU_RESPON_SERVER) {
      tungguRespon = false;
      u8g2.clearBuffer();
      iconBMP(5);
      drawWrappedText("Gagal Mengambil Data Server!", 72, screenHeight / 2, screenWidth * 0.75, u8g2_font_7x13_tf);
      u8g2.sendBuffer();
      buzzBasedOnMessage("500");
      delay(1000);
      client.disconnect();
      lastTunggurespon = millis();
    }
  }
}

void bukaAP(String _text) {
  modeAPaktif = true;
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

  u8g2.clearBuffer();
  drawWrappedText("MODE AKSES POIN", screenWidth / 2, 10, screenWidth, u8g2_font_luBIS08_tf);
  drawWrappedText(ssid, screenWidth / 2, 30, screenWidth, u8g2_font_7x13_tf);
  drawWrappedText(password ? password : "<Tidak Ada Password>", screenWidth / 2, 45, screenWidth, u8g2_font_7x13_tf);
  drawWrappedText(IP.toString().c_str(), screenWidth / 2, 60, screenWidth, u8g2_font_7x13_tf);
  u8g2.sendBuffer();
  buzz(2);
  client.disconnect();
}