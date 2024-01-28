#include <U8g2lib.h>
#include <SPI.h>

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);

void setup(void) {
  u8g2.begin();
}

void loop(void) {
  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "NAMA");
  u8g2.drawStr(0, 30, "Fatika X TE 2");
  u8g2.drawStr(0, 40, " Putri X TE 3");
  u8g2.drawStr(0, 50, " Rahmania X TE 2");
  u8g2.sendBuffer();
  delay(500);
}
