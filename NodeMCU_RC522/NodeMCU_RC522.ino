#include <SPI.h>
#include <MFRC522.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);  //Software I2C

constexpr uint8_t RST_PIN = D3;
constexpr uint8_t SS_PIN = D4;
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
String tag;
void setup() {
  Serial.begin(115200);
  SPI.begin();      // Init SPI bus
  rfid.PCD_Init();  // Init MFRC522

  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBIS08_tf);
  u8g2.drawStr(12, 10, "RFID Reader TEST");
  u8g2.drawStr(12, 30, "13,56 MHz");
  u8g2.drawStr(12, 50, "Tempelkan Kartu");
  u8g2.sendBuffer();
}
void loop() {
  if (!rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }

    Serial.println(tag);
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_luBIS08_tf);
    u8g2.drawStr(0, 10, "Hasil Baca RFID");
    u8g2.drawStr(0, 30, tag.c_str());
    u8g2.drawStr(12, 50, "Tempelkan Kartu");
    u8g2.sendBuffer();
    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}
