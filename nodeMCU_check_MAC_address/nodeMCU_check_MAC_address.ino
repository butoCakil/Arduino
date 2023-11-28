#include <ESP8266WiFi.h>
#include <MFRC522.h>

// RFID
#define SDA_PIN 2  // D4
#define RST_PIN 0  // D3

char chipID[25];
char IDTAG[20];

MFRC522 mfrc522(SDA_PIN, RST_PIN);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  Serial.println("Start");

  Serial.println("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Get ESP8266 Chip ID
  int num = ESP.getChipId();
  itoa(num, chipID, 10);

  // tampilkan idchip
  Serial.println("Chip ID: ");
  Serial.println(chipID);

  Serial.println("Finish");
  Serial.println();

  SPI.begin();
  mfrc522.PCD_Init();
}

void loop() {
  int berhasilBaca = bacaTag();

  if (berhasilBaca) {
    static char hasilTAG[20] = "";

    if (strcmp(hasilTAG, IDTAG) != 0) {
      strcpy(hasilTAG, IDTAG);
        Serial.println();
        Serial.println("=======================");
        Serial.println("ID Kartu: " + String(IDTAG));
        Serial.println("=======================");
        Serial.println("");
    }

    delay(1000);
    strcpy(hasilTAG, "");
  } else {
    strcpy(IDTAG, "");
  }
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
