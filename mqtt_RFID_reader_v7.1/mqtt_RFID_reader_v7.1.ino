#include <Arduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <MFRC522.h>
#include <WiFiClient.h>
#include <vector>

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
char nodevice[20] = "";

char* mqtt_server = "172.16.80.123";

const int mqtt_port = 1883;          // Port MQTT default
const char* mqtt_user = "ben";       // Username MQTT Anda
const char* mqtt_password = "1234";  // Password MQTT Anda

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);  //Software I2C

int screenWidth = u8g2.getWidth();
int screenHeight = u8g2.getHeight();

boolean aktifSerialMsg = false;
boolean tungguRespon = false;
boolean saatnyaRestart = false;
boolean modeAPaktif = false;
boolean terkirim = false;

#define BUZ_PIN D0  // D0 - MERAH
#define SET_BTN D8  // Push BUtton SET

// RFID
#define SDA_PIN 2  // D4
#define RST_PIN 0  // D3

char IDTAG[20];
char chipID[25];
char key[50] = "1234567890987654321";

struct WifiInfo {
  String ssid;
  int rssi;
  String security;
};

std::vector<WifiInfo> wifiList;

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
int tick = 0;
int networks;

// EEPROM Konfigurasi
const int eepromSize = 512;
const int debounceDelay = 50;  // Waktu debounce dalam milidetik

unsigned long buttonPressTime = 0;
boolean tombolDitekan = false;

String ssidNew = "", passNew, nodeviceNew, hostNew;
String usernameLogin, passwordLogin;

ESP8266WebServer server(80);

String injekHtml;

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
    formattedHtml.replace("%IDCHIP%", chipID);
    formattedHtml.replace("%c", "/");
    formattedHtml.replace("%SSID_NEW%", ssidNew);
    formattedHtml.replace("%PASS_NEW%", passNew);
    formattedHtml.replace("%NODEVICE%", nodevice);
    formattedHtml.replace("%HOST%", hostNew);

    if (networks == 0) {
      formattedHtml.replace("%find%", "");
      formattedHtml.replace("%IN%", "");
    } else {
      formattedHtml.replace("%find%", "<h5>SSID WiFi ditemukan (Klik SSID untuk memilih)</h5>");
      formattedHtml.replace("%IN%", injekHtml);
    }

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
    displayIconStatusText(ssidNew.c_str(), "Gagal simpan Config, Ulangi SET Config!", epd_bitmap_x_3x);


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
    displayIconStatusText(ssidNew.c_str(), "Berhasil simpan Config..", epd_bitmap_check_3x);
    delay(2000);  // Agar perangkat dapat mengirimkan data sebelum disconnect

    WiFi.softAPdisconnect(true);

    bootLoad("Menjalankan Config...");

    WiFi.mode(WIFI_STA);

    if (passNew.c_str()) {
      WiFi.begin(ssidNew.c_str(), passNew.c_str());
    } else {
      WiFi.begin(ssidNew.c_str());
    }

    // Tunggu sampai terhubung
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
      delay(1000);
      Serial.print("i");
      bootLoad("Menyambungkan ke WiFi...");
      attempts++;
    }

    if (WiFi.status() != WL_CONNECTED) {
      // Jika tidak terkoneksi dalam 30 detik,
      // tampilkan wifi tidak dapat terhubung
      buzz(3);
      modeAPaktif = true;
      displayIconStatusText(ssidNew.c_str(), "WiFi Gagal Konek.. RESET / Set AP", epd_bitmap_x_3x);
      delay(1000);
    } else {
      buzz(2);
      modeAPaktif = false;
      Serial.println("");
      Serial.println("Terhubung Ke Jaringan");
      displayIconStatusText(ssidNew.c_str(), "berhasil konek ke WiFi", epd_bitmap_check_3x);

      delay(2000);
      boot("Restart Device", "Tunggu...");
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
  u8g2.setBusClock(400000);  // Set bus speed to 400 kHz
  u8g2.setContrast(255);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Start");

  pinMode(BUZ_PIN, OUTPUT);
  pinMode(SET_BTN, INPUT);

  // Get ESP8266 Chip ID
  int num = ESP.getChipId();
  itoa(num, chipID, 10);

  // EEPROM config
  ssidNew = readStringFromEEPROM(0);
  passNew = readStringFromEEPROM(64);
  nodeviceNew = readStringFromEEPROM(192);
  hostNew = readStringFromEEPROM(256);

  boot("booting...", nodeviceNew.c_str());

  u8g2.clearBuffer();                  // clear the internal memory
  u8g2.setFont(u8g2_font_luBIS08_tf);  // choose a suitable font
  drawWrappedText("SMK Negeri Bansari", screenWidth / 2, 10, screenWidth, u8g2_font_luBIS08_tf);
  drawWrappedText("SiAPP", screenWidth / 2, (screenHeight / 2), screenWidth, u8g2_font_luBIS14_tf);
  u8g2.drawFrame(0, 16, 128, 1);
  u8g2.drawFrame(0, 33, 128, 1);
  drawWrappedText("Sistem Administrasi Presensi Pembelajaran", screenWidth / 2, 44, screenWidth, u8g2_font_luBIS08_tf);
  u8g2.sendBuffer();

  delay(3000);

  if (digitalRead(SET_BTN) == HIGH) {
    digitalWrite(D0, HIGH);
    delay(100);
    digitalWrite(D0, LOW);

    noLoadBarJustText("Tombol SET Aktif");
    Serial.println("Tombol Ditekan");

    modeAPaktif = true;

    startTimeBootLoad = millis();
    buzz(1);
    bootLoad("Memulai Mode AP");
    delay(1000);
    // Tombol ditekan selama 5 detik atau lebih, pindah ke mode Akses Poin
    bukaAP("Memulai mode Akses Poin...");
  } else {
    delay(2000);

    // Congig WiFi
    // Jika ada nilai SSID dan password di EEPROM, coba terhubung ke WiFi
    if (ssidNew != "" && passNew != "") {
      WiFi.mode(WIFI_STA);

      if (passNew.c_str()) {
        WiFi.begin(ssidNew.c_str(), passNew.c_str());
      } else {
        WiFi.begin(ssidNew.c_str());
      }

      int attempts = 0;
      startTimeBootLoad = millis();
      while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        Serial.println(attempts);

        u8g2.clearBuffer();
        drawWrappedText(ssidNew.c_str(), screenWidth / 2, 5, screenWidth, u8g2_font_luBIS08_tf);
        drawLoadingBar(screenWidth / 2, screenHeight / 2, 100, 10, 0.7);
        drawWrappedText("Menyambungkan WiFi", screenWidth / 2, 50, screenWidth, u8g2_font_7x13_tf);
        u8g2.sendBuffer();

        delay(1000);
        Serial.print(".");
        attempts++;
      }

      if (WiFi.status() == WL_CONNECTED) {
        modeAPaktif = false;
        Serial.println("");
        Serial.println("Terhubung Ke Jaringan");

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

        u8g2.clearBuffer();
        drawWrappedText(ssidNew.c_str(), (screenWidth / 2) - 5, 5, screenWidth, u8g2_font_luBIS08_tf);
        u8g2.drawXBM(52, 16, 24, 24, epd_bitmap_check_3x);
        drawWrappedText("Tersambung ke WiFi", screenWidth / 2, 50, screenWidth, u8g2_font_7x13_tf);
        u8g2.sendBuffer();

        delay(1000);

        // Setup MQTT client
        client.setServer(mqtt_server, mqtt_port);
        client.setCallback(callback);

        SPI.begin();
        mfrc522.PCD_Init();

        startTimeBootLoad = millis();
        while (!client.connected()) {
          noLoadBarJustTextTitle("Menyambungkan ke Server", hostNew.c_str());

          if (client.connect("NodeMCUClient", mqtt_user, mqtt_password)) {
            modeAPaktif = false;
            Serial.println("Tersambung ke MQTT Broker");

            u8g2.clearBuffer();
            drawWrappedText(hostNew.c_str(), (screenWidth / 2) - 5, 10, screenWidth, u8g2_font_luBIS08_tf);
            u8g2.drawXBM(52, 16, 24, 24, epd_bitmap_check_3x);
            drawWrappedText("Tersambung ke Server", screenWidth / 2, 50, screenWidth, u8g2_font_7x13_tf);
            u8g2.sendBuffer();
            buzzBasedOnMessage("200");
          } else {
            modeAPaktif = true;
            Serial.println("Koneksi MQTT gagal. Mengulangi koneksi...");
            noLoadBarJustText("Mengulang koneksi Server..");
            delay(1000);
            buzz(1);
          }
        }

        // Subscribe to a topic
        String topic = "responServer_";
        topic += nodevice;
        client.subscribe(topic.c_str(), 1);

        buzz(2);

        Serial.println("Tempelkan kartu RFID..");
      } else {
        modeAPaktif = true;
        buzz(1);
        bootLoad("Memulai Mode AP");
        delay(1000);
        // Tombol ditekan selama 5 detik atau lebih, pindah ke mode Akses Poin
        bukaAP("Memulai mode Akses Poin...");
      }
    } else {
      modeAPaktif = true;
      bootLoad("Memulai Mode AP");
      delay(1000);
      bukaAP("SSID dan password tidak ditemukan di EEPROM. Memulai mode Akses Poin...");
    }
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/setting", handelLogin);
  server.on("/action_page", HTTP_POST, handleForm);
  server.on("/reboot", handleReboot);
  server.onNotFound(handleNotFound);

  client.disconnect();
  buzz(2);
}

void loop() {
  client.loop();

  if (WiFi.status() != WL_CONNECTED) {
    server.handleClient();
  }

  if (modeAPaktif == false) {
    homeLCD();
  }

  int berhasilBaca = bacaTag();
  checkButton();

  if (berhasilBaca) {
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

  if (tungguRespon) {
    unsigned long currentTime2 = millis();
    if (currentTime2 - lastTunggurespon > TUNGGU_RESPON_SERVER) {
      tungguRespon = false;

      u8g2.clearBuffer();

      if (terkirim) {
        iconBMP(5);
        drawWrappedText("Dicoba lagi! Server tidak mengirim Respon...", 72, screenHeight / 2, screenWidth * 0.75, u8g2_font_7x13_tf);
      } else {
        iconBMP(2);
        drawWrappedText("Data Gagal kirim. Request Timeout... ", 72, screenHeight / 2, screenWidth * 0.75, u8g2_font_7x13_tf);
      }
      u8g2.sendBuffer();

      buzzBasedOnMessage("500");
      delay(1000);
      client.disconnect();
      lastTunggurespon = millis();
      u8g2.clearBuffer();
      client.disconnect();
      terkirim = false;
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

  searchingWifi();

  String ssidString = "SiAPP-" + String(chipID);
  const char* ssid = ssidString.c_str();
  const char* password = "siap$bos";

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  server.begin();  // Mulai server dalam mode Akses Poin

  delay(1000);

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