#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

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

// Ganti true jika berhotspot tanpa password
boolean modeHotspot = false;
const char* hotspot = "HOTSPOT-SKANEBA";

// HOTSPOT ITC
// const char* ssid = "HOTSPOT-SKANEBA";
// const char* password = "itbisa123";

// WiFi credentials
// ASSEMBLY TE - CNC
const char* ssid = "ASSEMBLY ONLY";
const char* password = "onlyassemblytebos";

// INSTRUKTUR TE - 2.4G
// const char* ssid = "INTRUKTUR-TAV-2.4G";
// const char* password = "skanebabisa1";

//HOSTSPOT-SKANEBA-TU
// const char* ssid = "HOTSPOT-SKANEBA-TU";
// const char* password = "skanebabisa";

// MQTT Broker Configuration
char nodevice[20] = "2309G005";  // GERBANG / PRESENSI MASUK (max 20 characters)
// char nodevice[20] = "2309MAS005";  // PEMBIASAAN MASJID (max 20 characters)
// char nodevice[20] = "2310IZ002";  // POS SATPAM (IJIN) (max 20 characters)
// char nodevice[20] = "2309NA003";  // PEMBIASAAN MASJID (max 20 characters)

const char* mqtt_server = "172.16.80.123";  // Ganti dengan alamat IP broker MQTT Anda
// const char* mqtt_server = "10.16.0.102";  // Ganti dengan alamat IP broker MQTT Anda

const int mqtt_port = 1883;          // Port MQTT default
const char* mqtt_user = "ben";       // Username MQTT Anda
const char* mqtt_password = "1234";  // Password MQTT Anda

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);  //Software I2C

boolean aktifSerialMsg = false;
boolean tungguRespon = false;
boolean saatnyaRestart = false;

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

int nom;

int screenWidth = u8g2.getWidth();
int screenHeight = u8g2.getHeight();

#include "function.h"

void setup() {
  u8g2.begin();
  Serial.begin(115200);
  Serial.println();
  Serial.println("Start");

  // Set bus speed to 400 kHz
  u8g2.setBusClock(400000);
  u8g2.setContrast(255); // Set contrast to maximum

  u8g2.clearBuffer();                  // clear the internal memory
  u8g2.setFont(u8g2_font_luBIS08_tf);  // choose a suitable font
  drawWrappedText("SIAPP", screenWidth / 2, 10, screenWidth, u8g2_font_luBIS08_tf);
  u8g2.sendBuffer();

  boot("booting...");

  // pinMode(LED_PIN, OUTPUT);
  pinMode(BUZ_PIN, OUTPUT);
  // pinMode(OKE_PIN, OUTPUT);
  pinMode(SET_BTN, INPUT_PULLUP);

  if (modeHotspot == true) {
    const char* ssid = hotspot;
  }

  if (modeHotspot == true) {
    WiFi.begin(ssid);
  } else {
    WiFi.begin(ssid, password);
  }

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    bootLoad("Menyambungkan WiFi...");
  }

  // digitalWrite(LED_PIN, HIGH);
  Serial.println();
  Serial.println("Tersambung ke WiFi");
  Serial.println();

  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Get ESP8266 Chip ID
  int num = ESP.getChipId();
  itoa(num, chipID, 10);
  Serial.println("Chip ID: ");
  Serial.println(chipID);

  displayIconStatusText(ssid, "Tersambung ke WiFi", epd_bitmap_check_3x);
  delay(1000);

  // Setup MQTT client
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  SPI.begin();
  mfrc522.PCD_Init();

  while (!client.connected()) {
    bootLoad("Menyambungkan ke Server..");
    if (client.connect("NodeMCUClient", mqtt_user, mqtt_password)) {
      Serial.println("Tersambung ke MQTT Broker");
      buzzBasedOnMessage("200");
    } else {
      Serial.println("Koneksi MQTT gagal. Mengulangi koneksi...");

      displayIconStatusText("SIAPP", "Gagal konek Server!", epd_bitmap_x_3x);
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

  buzz(3);

  Serial.println("Tempelkan kartu RFID");
}

void loop() {
  int berhasilBaca = bacaTag();

  client.loop();
  homeLCD();

  if (berhasilBaca) {
    lastRFIDReadTime = millis();    // Perbarui waktu terakhir pembacaan kartu RFID
    static char hasilTAG[20] = "";  // Store previous tag ID

    if (strcmp(hasilTAG, IDTAG) != 0) {
      buzz(1);
      noLoadBarJustText("Membaca ID Kartu");
      reconnect();

      strcpy(hasilTAG, IDTAG);

      if (aktifSerialMsg) {
        Serial.println("ID Kartu: " + String(IDTAG));
      }

      // Send data to server and receive JSON response
      String jsonResponse = sendCardIdToServer(IDTAG);

      tungguRespon = true;
      lastTunggurespon = millis();

      if (aktifSerialMsg) {
        Serial.println("Selesai kirim untuk ID: " + String(IDTAG));
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
  u8g2.drawXBM(112, 0, 16, 16, epd_bitmap_loop_circular_2x_120);
  u8g2.sendBuffer();

  // Periksa apakah sudah waktunya untuk restart
  unsigned long currentTime = millis();
  if (currentTime - lastRFIDReadTime > RFID_READ_INTERVAL) {
    saatnyaRestart = true;
  }

  if (saatnyaRestart) {
    Serial.println("Tidak ada aktifitas pembacaan kartu RFID selama 10 menit. Melakukan restart...");
    boot("Restart dalam 3 detik");
    buzzBasedOnMessage("400");
    delay(1000);
    ESP.restart();
  }

  if (tungguRespon) {
    unsigned long currentTime2 = millis();
    if (currentTime2 - lastTunggurespon > TUNGGU_RESPON_SERVER) {
      tungguRespon = false;
      u8g2.clearBuffer();
      iconBMP(5);
      drawWrappedText("Gagal Mengambil Data Server!", 72, screenHeight / 2, screenWidth * 0.75, u8g2_font_7x13_tf);
      u8g2.sendBuffer();
      client.disconnect();
      lastTunggurespon = millis();
    }
  }
}