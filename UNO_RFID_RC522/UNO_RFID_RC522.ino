#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9   // Tentukan pin untuk RST
#define SDA_PIN 10  // Tentukan pin untuk SDA

MFRC522 mfrc522(SDA_PIN, RST_PIN);  // Buat objek MFRC522

char nokartu[20];

void setup() {
  Serial.begin(9600);  // Mulai Serial dengan kecepatan 9600 bps
  SPI.begin();         // Mulai SPI bus
  mfrc522.PCD_Init();  // Mulai MFRC522
  Serial.println("Baca Kartu RFID");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent())
    return;

  if (!mfrc522.PICC_ReadCardSerial())
    return;

  memset(nokartu, 0, sizeof(nokartu));

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    snprintf(nokartu, sizeof(nokartu), "%s%02X", nokartu, mfrc522.uid.uidByte[i]);
  }

  Serial.print("Nokartu: ");
  Serial.println(nokartu);
}
