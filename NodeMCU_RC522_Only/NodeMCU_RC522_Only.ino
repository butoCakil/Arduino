#include <SPI.h>
#include <MFRC522.h>

String kunciID = "5840211102";

#define BUZ_PIN 16 // D0
#define RELAY_PIN 5 // D1

boolean bukaKunci = false; // false = 0 , true = 1

constexpr uint8_t RST_PIN = 0; // D3
constexpr uint8_t SS_PIN = 2; // D4

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

String tag;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(BUZ_PIN, OUTPUT);
  digitalWrite(BUZ_PIN, LOW);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  Serial.println("");
  Serial.println("");
  Serial.println("Tempelkan Kartu!");
}
void loop() {
  if (bukaKunci) {
    // AKTIF
    digitalWrite(RELAY_PIN, LOW);
  } else {
    // Jika tidak
    // MATI
    digitalWrite(RELAY_PIN, HIGH);
  }

  if (!rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }

    buzzer(1, 100);

    Serial.print("Nomor ID: ");
    Serial.println(tag);

    if (tag == kunciID) {
      // Jika Kunci Cocok
      bukaKunci = !bukaKunci;

      buzzer(2, 100);

      Serial.println("KUNCI SESUAI");
      Serial.println("");
      Serial.println("BERHASIL DIBUKA!");
    } else {
      bukaKunci = false;

      buzzer(3, 1000);

      Serial.println("KUNCI SALAH!");
      Serial.println("");
      Serial.println("Tempelkan Kartu yang BENAR!");
    }

    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

void buzzer(int _jmlTit, int _waktuTit) {
  for (int i = 0; i < _jmlTit; i++) {
    digitalWrite(BUZ_PIN, HIGH);
    delay(_waktuTit);
    digitalWrite(BUZ_PIN, LOW);
    delay(100);
  }
}
