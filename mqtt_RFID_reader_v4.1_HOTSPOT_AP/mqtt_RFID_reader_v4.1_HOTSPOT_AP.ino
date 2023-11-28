#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <MFRC522.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#include "index.h"
#include "login.h"
#include "error.h"

boolean aktifSerialMsg = true;
boolean autoRestart = false;
boolean tungguRespon = false;
boolean saatnyaRestart = false;
boolean modeAPaktif = false;

char nodevice[20] = "";

// const char* mqtt_server;
const int MAX_EEPROM_SIZE = 256;
char mqtt_server[MAX_EEPROM_SIZE];

const int mqtt_port = 1883;          // Port MQTT default
const char* mqtt_user = "ben";       // Username MQTT Anda
const char* mqtt_password = "1234";  // Password MQTT Anda

#define LED_PIN D0  // D0 - MERAH
#define BUZ_PIN D1  // D1 - BIRU - BUZZER
#define OKE_PIN D2  // D2 - HIJAU
#define SET_BTN D8  // Push BUtton SET

// RFID
#define SDA_PIN 2  // D4
#define RST_PIN 0  // D3

char IDTAG[20];
char chipID[25];                       // Store ESP8266 Chip ID
char key[50] = "1234567890987654321";  // Change to your desired Key (max 20 characters)

// Membuat array untuk memetakan pesan ke kode bunyi buzzer
const char* buzzerCodes[] = {
  // error
  "400", "_ _",
  "404", "_..._",
  "405", "_...._",
  "406", "_....._",
  "407", "_....._",
  "500", "_....._",
  "501", "_._..._",
  "502", "_._....._",
  "505", "_....._._",
  "515", "_....._",
  "545", "_....._",
  "555", "_....._",
  // akses
  "IDTT", "._..",
  "HLTM", "_.",
  "TBPS", "._._..",
  "TASK", "_._..",
  "PLAW", "_..",
  // OK
  "200", "..",
  "SAPP", "...",
  "PPBH", "..",
  "510", "...",
  "PPPP", "...",
  "SMPM", "...",
  "MMMM", "...",
  "BMPM", "..",
  "PKBD", "..",
  "BMPE", "..",
  "BPSE", "..",
  "BPEB", "..",
  "BMIJ", ".."
};

String receivedMessage = "";

MFRC522 mfrc522(SDA_PIN, RST_PIN);

WiFiClient espClient;
PubSubClient client(espClient);

// Variabel untuk memantau waktu terakhir pembacaan kartu RFID
unsigned long lastRFIDReadTime = 0;
const unsigned long RFID_READ_INTERVAL = 600000;  // 10 menit dalam milidetik (10 * 60 * 1000)

unsigned long previousLEDmqtt = 0;
const long intervalLEDmqtt = 100;
const long intervalLEDmqtt2 = 500;

int tick = 0;

// EEPROM Konfigurasi
const int eepromSize = 512;
const int debounceDelay = 50;  // Waktu debounce dalam milidetik

unsigned long buttonPressTime = 0;
boolean tombolDitekan = false;

// const char* ssid = "SiAPP-Config";
// const char* password = "siap$bos";

String ssidNew = "", passNew, nodeviceNew, hostNew;
String usernameLogin, passwordLogin;

ESP8266WebServer server(80);

// function handel EEPROM dan page Akses poin
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
    buzz(1);
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

      buzz(2);
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

  // EEPROM config
  ssidNew = readStringFromEEPROM(0);
  passNew = readStringFromEEPROM(64);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZ_PIN, OUTPUT);
  pinMode(OKE_PIN, OUTPUT);
  pinMode(SET_BTN, INPUT);

  // Get ESP8266 Chip ID
  int num = ESP.getChipId();
  itoa(num, chipID, 10);

  // Congig WiFi
  // Jika ada nilai SSID dan password di EEPROM, coba terhubung ke WiFi
  if (ssidNew != "" && passNew != "") {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssidNew.c_str(), passNew.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
      Serial.print(".");
      blink(3, 100, 400);
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      modeAPaktif = false;
      Serial.println("");
      Serial.println("Terhubung Ke Jaringan");

      ssidNew = readStringFromEEPROM(0);
      passNew = readStringFromEEPROM(64);
      nodeviceNew = readStringFromEEPROM(192);
      hostNew = readStringFromEEPROM(MAX_EEPROM_SIZE);

      strcpy(nodevice, nodeviceNew.length() > 0 ? nodeviceNew.c_str() : nodevice);

      hostNew.toCharArray(mqtt_server, sizeof(mqtt_server));

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

      int attempts = 0;
      while (!client.connected() && attempts < 30) {
        if (client.connect("NodeMCUClient", mqtt_user, mqtt_password)) {
          modeAPaktif = false;
          if (aktifSerialMsg)
            Serial.println("Tersambung ke MQTT Broker");

          buzzBasedOnMessage("200");
        } else {
          modeAPaktif = true;
          Serial.println("Koneksi MQTT gagal. Mengulangi koneksi...");
          blink(5, 100, 1000);
          ledSuccess(false);
          checkButton();
          if(tombolDitekan){
            break;
          }
        }

        attempts++;
      }

      // Subscribe to a topic
      String topic = "responServer_";
      topic += nodevice;
      client.subscribe(topic.c_str(), 0);

      buzz(2);

      Serial.println("Tempelkan kartu RFID..");
    } else {
      modeAPaktif = true;
      delay(1000);
      bukaAP("Tidak Bisa konek ke Jaringan Wifi. modeAP aktif.");
    }
  } else {
    modeAPaktif = true;
    delay(1000);
    Serial.println("SSID dan password tidak ditemukan di EEPROM. Memulai mode Akses Poin...");
    bukaAP("SSID dan password tidak ditemukan di EEPROM. Memulai mode Akses Poin...");
  }

  Serial.println("");
  Serial.println("Device Hampir Siap");

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

  if (!client.connected() && modeAPaktif) {
    server.handleClient();
  }

  checkButton();
  int berhasilBaca = bacaTag();

  client.loop();

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

  if (berhasilBaca) {
    tick = 0;
    if (IDTAG) {
      buzz(1);
      reconnect();
    }

    if (autoRestart)
      lastRFIDReadTime = millis();  // Perbarui waktu terakhir pembacaan kartu RFID

    static char hasilTAG[20] = "";  // Store previous tag ID

    if (strcmp(hasilTAG, IDTAG) != 0) {
      strcpy(hasilTAG, IDTAG);

      if (aktifSerialMsg) {
        Serial.println();
        Serial.println("=======================");
        Serial.println("ID Kartu: " + String(IDTAG));
      }

      // Send data to server and receive JSON response
      String jsonResponse = sendCardIdToServer(IDTAG);

      digitalWrite(OKE_PIN, LOW);
      digitalWrite(LED_PIN, LOW);

      if (aktifSerialMsg) {
        Serial.println("Selesai kirim untuk ID: " + String(IDTAG));
        Serial.println("=======================");
        Serial.println("");
      }

    } else {
      Serial.println("-");
    }

    delay(900);
    strcpy(hasilTAG, "");
  } else {
    strcpy(IDTAG, "");
  }

  buzz(0);
  receivedMessage = "";

  if (autoRestart) {
    // Periksa apakah sudah waktunya untuk restart
    unsigned long currentTime = millis();
    if (currentTime - lastRFIDReadTime > RFID_READ_INTERVAL) {
      Serial.println("Tidak ada aktifitas pembacaan kartu RFID selama 10 menit. Melakukan restart...");
      ESP.restart();
    }
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Menerima pesan pada topic: ");
  Serial.println(topic);
  for (int i = 0; i < length; i++) {
    receivedMessage += (char)payload[i];
  }

  if (aktifSerialMsg) {
    Serial.print("Pesan: ");
    Serial.println(receivedMessage);
    Serial.println();
  }

  // prosen respon data
  identifyAndProcessJsonResponse(receivedMessage, nodevice);
}

void bukaAP(String _text) {
  modeAPaktif = true;
  Serial.println("");
  Serial.println(_text);
  WiFi.disconnect();
  // delay sebelum memulai konfigurasi AP
  delay(1000);

  String ssidString = "SiAPP-Config-" + String(chipID);
  const char* _ssid = ssidString.c_str();
  const char* _password = "siap$bos";

  WiFi.mode(WIFI_AP);
  WiFi.softAP(_ssid, _password);
  server.begin();  // Mulai server dalam mode Akses Poin

  IPAddress IP = WiFi.softAPIP();
  Serial.println("");
  Serial.print("AP: ");
  Serial.println(_ssid);
  Serial.print("Password: ");
  Serial.println(_password);
  Serial.print("IP Address (AP): ");
  Serial.println(IP);

  buzz(3);
  client.disconnect();
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

String sendCardIdToServer(String cardId) {
  String jsonResponse = "";
  // Send RFID card data, Chip ID, Node Device, and Key to the server
  String request = "{";
  request += "\"nokartu\":\"" + String(cardId) + "\",";
  request += "\"idchip\":\"" + String(chipID) + "\",";
  request += "\"nodevice\":\"" + String(nodevice) + "\",";
  request += "\"key\":\"" + String(key) + "\",";
  request += "\"ipa\":\"" + WiFi.localIP().toString() + "\"";
  request += "}";

  if (client.connect("NodeMCUClient", mqtt_user, mqtt_password)) {
    String mqttTopic = "dariMCU_" + String(nodevice);
    Serial.println("Tersambung ke MQTT Broker");
    Serial.println("Kirim ke topik: " + mqttTopic + ": " + request);
    client.publish(mqttTopic.c_str(), request.c_str(), 0);

    ledSuccess(true);
  } else {
    ledSuccess(false);

    buzzBasedOnMessage("400");
    Serial.println("Koneksi ke MQTT Broker gagal");

    reconnect();
  }

  return jsonResponse;
}

void reconnect() {
  // Loop sampai terhubung ke broker MQTT
  while (!client.connected()) {
    ledSuccess(false);

    Serial.println("Menyambungkan ke MQTT Broker...");
    // Coba terhubung ke broker MQTT
    if (client.connect("NodeMCUClient", mqtt_user, mqtt_password)) {

      Serial.println("Tersambung ke MQTT Broker");

      // Langganan topik yang Anda butuhkan di sini jika diperlukan
      String topic = "responServer_";
      topic += nodevice;
      client.subscribe(topic.c_str(), 0);

      ledSuccess(true);
    } else {
      ledSuccess(false);

      buzzBasedOnMessage("400");

      Serial.print("MQTT Gagal, rc=");
      Serial.print(client.state());
      Serial.println(" mencoba konek lagi dalam 5 detik");
      // Tunggu sebelum mencoba lagi
      blink(5, 100, 2000);
    }
  }
}

void buzz(int loop) {
  if (loop == 0) {
    digitalWrite(BUZ_PIN, LOW);
  } else if (loop == 1) {
    digitalWrite(BUZ_PIN, HIGH);
    delay(100);
    digitalWrite(BUZ_PIN, LOW);
  } else {
    for (int t = 0; t < loop; t++) {
      digitalWrite(BUZ_PIN, HIGH);
      delay(100);
      digitalWrite(BUZ_PIN, LOW);
      delay(100);
    }
  }
}

void blink(int _loop, int ms_, int _ms) {
  for (int r = 0; r < _loop; r++) {
    digitalWrite(LED_PIN, HIGH);
    delay(ms_);
    digitalWrite(LED_PIN, LOW);
    delay(ms_);
  }
  delay(_ms);
}

void ledSuccess(bool state) {
  if (state) {
    digitalWrite(OKE_PIN, HIGH);
    digitalWrite(LED_PIN, LOW);
  } else {
    digitalWrite(OKE_PIN, LOW);
    digitalWrite(LED_PIN, HIGH);
  }
}

void buzz_er(String _kode) {
  for (int i = 0; i < _kode.length(); i++) {
    char karakter = _kode.charAt(i);

    if (karakter == '_') {
      // Buzzer berbunyi selama 1 detik
      // Frekuensi bunyi buzzer (1 kHz)
      // tone(BUZ_PIN, 1000);
      digitalWrite(BUZ_PIN, HIGH);
      // Bunyi selama 1 detik
      delay(1000);
      // Matikan buzzer
      // noTone(BUZ_PIN);
      digitalWrite(BUZ_PIN, LOW);
    } else if (karakter == '.') {
      // Buzzer berbunyi selama 100 mili detik
      // Frekuensi bunyi buzzer (1 kHz)
      // tone(BUZ_PIN, 1000);
      digitalWrite(BUZ_PIN, HIGH);
      // Bunyi selama 100 mili detik
      delay(100);
      // Matikan buzzer
      // noTone(BUZ_PIN);
      digitalWrite(BUZ_PIN, LOW);
    } else if (karakter == ' ') {
      // Tunda 100 mili detik
      delay(100);
    }

    // Tunda sebelum karakter selanjutnya (jika ada)
    if (i < _kode.length() - 1) {
      delay(100);  // Tunda 100 mili detik sebelum karakter berikutnya
    }
  }

  delay(1000);
}

void buzzBasedOnMessage(const char* message) {
  for (int i = 0; i < sizeof(buzzerCodes) / sizeof(buzzerCodes[0]); i += 2) {
    if (strcmp(message, buzzerCodes[i]) == 0) {
      buzz_er(buzzerCodes[i + 1]);
      Serial.println(buzzerCodes[i + 1]);
      break;  // Keluar dari loop setelah menemukan kode yang cocok
    }
  }
}

void identifyAndProcessJsonResponse(String jsonResponse, char* _nodevice) {
  const char* pesanJSON = "";

  // Parse and process JSON response
  // Menghapus karakter newline dan carriage return
  jsonResponse.replace("\n", "");
  jsonResponse.replace("\r", "");
  // Menghapus karakter backslash yang mengganggu
  jsonResponse.replace("\\", "");

  // Menghapus karakter ganda ("") dari awal dan akhir JSON
  jsonResponse = jsonResponse.substring(1, jsonResponse.length() - 1);
  DynamicJsonDocument jsonDoc(1024);
  DeserializationError error = deserializeJson(jsonDoc, jsonResponse);

  Serial.println("");
  Serial.print("JSON Respon: ");
  Serial.println(jsonResponse);
  Serial.println("");

  if (error) {
    ledSuccess(false);

    pesanJSON = "500";
    Serial.print("gagal to parse JSON: ");
    Serial.println(error.c_str());
  } else {
    // Mengakses elemen-elemen JSON yang benar
    const char* json_id = jsonDoc["respon"][0]["id"];
    const char* json_nodevice = jsonDoc["respon"][0]["nodevice"];
    const char* json_message = jsonDoc["respon"][0]["message"];
    const char* json_info = jsonDoc["respon"][0]["info"];
    const char* json_nokartu = jsonDoc["respon"][0]["nokartu"];

    if (json_nodevice) {
      if (aktifSerialMsg) {
        Serial.print("- id: ");
        Serial.println(json_id);
        Serial.print("- nodevice asal: ");
        Serial.println(_nodevice);
        Serial.print("- nodevice json: ");
        Serial.println(json_nodevice);
        Serial.print("- pesan: ");
        Serial.println(json_message);
        Serial.print("- info: ");
        Serial.println(json_info);
        Serial.print("- nokartu: ");
        Serial.println(json_nokartu);
      }

      if (strcmp(_nodevice, json_nodevice) == 0 && strcmp("406", json_message) != 0) {
        ledSuccess(true);

        if (aktifSerialMsg) {
          Serial.print("ID & Nomor Device Sesuai! ");
          Serial.println();
        }

        pesanJSON = json_message;
      } else {
        ledSuccess(false);

        Serial.println("ID & Nomor Device Tidak Sesuai...!");
        Serial.println("Permintaan tidak direspon.");

        pesanJSON = "501";
      }
    } else {
      ledSuccess(false);

      // Elemen "nodevice" tidak ada dalam JSON
      Serial.println("Elemen \"nodevice\" tidak ada dalam JSON.");
      Serial.println("Permintaan tidak direspon.");

      pesanJSON = "502";
    }
  }

  // Setelah selesai memproses respon, putuskan koneksi MQTT
  client.disconnect();
  Serial.println("Koneksi Selesai");

  // aktikan Buzz sesuai KOde Pesan
  buzzBasedOnMessage(pesanJSON);
}