#include <Arduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <MFRC522.h>
#include <WiFiClient.h>
#include <vector>

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

boolean aktifSerialMsg = true;
boolean autoRestart = false;
boolean tungguRespon = false;
boolean saatnyaRestart = false;
boolean modeAPaktif = false;

#define BUZ_PIN D1  // D0 - MERAH BUZZER
#define LED_PIN D0  // D0 - BIRU
#define OKE_PIN D2  // D2 - HIJAU
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

unsigned long previousLEDmqtt = 0;
const long intervalLEDmqtt = 100;
const long intervalLEDmqtt2 = 500;

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
      delay(1000);
    } else {
      buzz(2);
      modeAPaktif = false;
      Serial.println("");
      Serial.println("Terhubung Ke Jaringan");

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
      delay(1000);
      // Tombol ditekan selama 5 detik atau lebih, pindah ke mode Akses Poin
      bukaAP("Memulai mode Akses Poin...");
    }
  } else {
    tombolDitekan = false;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Start");

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZ_PIN, OUTPUT);
  pinMode(OKE_PIN, OUTPUT);
  pinMode(SET_BTN, INPUT);

  // Get ESP8266 Chip ID
  int num = ESP.getChipId();
  itoa(num, chipID, 10);

  delay(3000);

  // EEPROM config
  ssidNew = readStringFromEEPROM(0);
  passNew = readStringFromEEPROM(64);

  if (digitalRead(SET_BTN) == HIGH) {
    digitalWrite(D0, HIGH);
    delay(100);
    digitalWrite(D0, LOW);

    Serial.println("Tombol Ditekan");

    modeAPaktif = true;

    startTimeBootLoad = millis();
    buzz(1);
    delay(1000);
    // Tombol ditekan selama 5 detik atau lebih, pindah ke mode Akses Poin
    bukaAP("Memulai mode Akses Poin...");
  } else {
    delay(2000);

    // Congig WiFi
    // Jika ada nilai SSID dan password di EEPROM, coba terhubung ke WiFi
    if (ssidNew != "" && passNew != "") {
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssidNew.c_str(), passNew.c_str());

      int attempts = 0;
      startTimeBootLoad = millis();
      while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        Serial.println(attempts);
        delay(1000);
        Serial.print(".");
        attempts++;
      }

      // if (WiFi.status() != WL_CONNECTED) {
      //   Serial.println("Gagal Konek Wifi");
      //   // displayIconStatusText(ssidNew.c_str(), "WiFi Gagal Konek.. RESET / Set AP", epd_bitmap_x_3x);
      //   // delay(2000);
      //   bukaAP("wifi gagal konek. Memulai mode Akses Poin...");
      // }

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
          if (client.connect("NodeMCUClient", mqtt_user, mqtt_password)) {
            modeAPaktif = false;
            if (aktifSerialMsg)
              Serial.println("Tersambung ke MQTT Broker");

            buzzBasedOnMessage("200");
          } else {
            modeAPaktif = true;
            Serial.println("Koneksi MQTT gagal. Mengulangi koneksi...");
          }
        }

        // Subscribe to a topic
        String topic = "responServer_";
        topic += nodevice;
        client.subscribe(topic.c_str(), 0);

        buzz(2);

        Serial.println("Tempelkan kartu RFID..");
      } else {
        modeAPaktif = true;
        buzz(1);
        delay(1000);
        // Tombol ditekan selama 5 detik atau lebih, pindah ke mode Akses Poin
        bukaAP("Memulai mode Akses Poin...");
      }
    } else {
      modeAPaktif = true;
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
  if (WiFi.status() != WL_CONNECTED) {
    server.handleClient();
  }

  checkButton();
  int berhasilBaca = bacaTag();

  client.loop();

  if (berhasilBaca) {
    if (autoRestart) {
      lastRFIDReadTime = millis();  // Perbarui waktu terakhir pembacaan kartu RFID
    }

    static char hasilTAG[20] = "";  // Store previous tag ID

    if (strcmp(hasilTAG, IDTAG) != 0) {
      buzz(1);
      reconnect();

      strcpy(hasilTAG, IDTAG);

      if (aktifSerialMsg)
        Serial.println("ID Kartu: " + String(IDTAG));

      // Send data to server and receive JSON response
      String jsonResponse = sendCardIdToServer(IDTAG);

      digitalWrite(OKE_PIN, LOW);
      digitalWrite(LED_PIN, LOW);

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

  // Periksa apakah sudah waktunya untuk restart
  if (autoRestart) {
    unsigned long currentTime = millis();
    if (currentTime - lastRFIDReadTime > RFID_READ_INTERVAL) {
      saatnyaRestart = true;
    }

    if (saatnyaRestart) {
      if (aktifSerialMsg)
        Serial.println("Tidak ada aktifitas pembacaan kartu RFID selama 10 menit. Melakukan restart...");

      buzzBasedOnMessage("400");
      delay(1000);
      ESP.restart();
    }
  }

  if (tungguRespon) {
    unsigned long currentTime2 = millis();
    if (currentTime2 - lastTunggurespon > TUNGGU_RESPON_SERVER) {
      tungguRespon = false;
      buzzBasedOnMessage("500");
      delay(1000);
      client.disconnect();
      lastTunggurespon = millis();
    }
  }

  standBy();
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

  buzz(2);
  client.disconnect();
}

void standBy() {
  if (modeAPaktif == false) {
    // Ambil waktu sekarang
    unsigned long currentMillisLEDmqtt = millis();

    // Indikator Koneksi LED JIka konek Kedil Slow, Jika diskonek kedip cepet
    if (!client.connected()) {
      if (tick < 1) {
        tick = 1;
        Serial.println("[Stanby] Tidak terkoneksi ke Server");
        Serial.println();
      }
      tick++;
      digitalWrite(OKE_PIN, LOW);

      if (currentMillisLEDmqtt - previousLEDmqtt >= intervalLEDmqtt2) {
        previousLEDmqtt = currentMillisLEDmqtt;

        if (digitalRead(LED_PIN) == LOW) {
          digitalWrite(LED_PIN, HIGH);
        } else {
          digitalWrite(LED_PIN, LOW);
        }
      }
    } else {
      if (tick < 1) {
        tick = 1;
        Serial.println("Menyambungkan ke Server.. ");
        Serial.println();
      }
      tick++;

      if (currentMillisLEDmqtt - previousLEDmqtt >= intervalLEDmqtt) {
        previousLEDmqtt = currentMillisLEDmqtt;

        if (digitalRead(LED_PIN) == LOW) {
          digitalWrite(LED_PIN, HIGH);
        } else {
          digitalWrite(LED_PIN, LOW);
        }
      }
    }
  } else {
    // logik LED mode AP
    unsigned long mulaiBlink = millis();

    if (mulaiBlink - previousLEDmqtt > 1000) {
      previousLEDmqtt = mulaiBlink;

      if (digitalRead(LED_PIN) == LOW) {
        digitalWrite(LED_PIN, HIGH);
      } else {
        digitalWrite(LED_PIN, LOW);
      }

      if (digitalRead(OKE_PIN) == LOW) {
        digitalWrite(OKE_PIN, HIGH);
      } else {
        digitalWrite(OKE_PIN, LOW);
      }
    }
  }
}