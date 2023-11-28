#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>

// Konfigurasi WiFi
const char *ssid = "ASSEMBLY ONLY";
const char *password = "onlyassemblytebos";

// Konfigurasi RC522
#define RST_PIN 22  // RST-PIN RC522 - GPIO 22
#define SS_PIN 21   // SDA-PIN RC522 - GPIO 21
MFRC522 mfrc522(SS_PIN, RST_PIN);

char IDTAG[18];  // Buffer untuk menyimpan ID tag RFID

void setup() {
  Serial.begin(115200);

  // Koneksi ke WiFi
  connectToWiFi();

  // Tampilkan informasi IP dan MAC
  showConnectionInfo();

  // Inisialisasi RC522
  mfrc522.PCD_Init();
}

void loop() {
  // Baca tag RFID
  if (bacaTag()) {
    Serial.print("ID Tag: ");
    Serial.println(IDTAG);
  }
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Jika terhubung, coba perbarui alamat IP dengan alamat yang diinginkan
  IPAddress customIP(10, 16, 0, 123);  // Ganti ini dengan alamat IP yang diinginkan
  IPAddress subnet(255, 255, 255, 0);
  IPAddress gateway(10, 16, 0, 254);  // Ganti ini dengan gateway yang sesuai

  WiFi.config(customIP, gateway, subnet);

  Serial.println("Connected to WiFi");
}

void showConnectionInfo() {
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  Serial.print("Chip ID: ");
  Serial.println(idChip());

  idChip();
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