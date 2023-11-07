#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 22
#define RST_PIN 21

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

String strID;

void setup() {
  // put your setup code here, to run once:
  SPI.begin();
  rfid.PCD_Init();
  Serial.begin(9600);
  Serial.println("I am waiting for card...");
}

void loop() {
  // put your main code here, to run repeatedly:
if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  // Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  // Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  //id kartu dan yang akan dikirim ke database
  strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
      (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
      String(rfid.uid.uidByte[i], HEX) +
      (i != 3 ? ":" : "");
  }

  strID.toUpperCase();
  Serial.print("Kartu ID Anda : ");
  Serial.println(strID);
  delay(1000);
}
