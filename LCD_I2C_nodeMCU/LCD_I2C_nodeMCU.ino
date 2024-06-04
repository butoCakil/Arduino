#include <LiquidCrystal_I2C.h>

// LiquidCrystal_I2C lcd(0x3F, 16, 2);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // 16x2

int hitung;

void setup() {
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
}

void loop() {
  hitung++;

  if (hitung > 99)
    hitung = 0;

  lcd.setCursor(0, 0);
  lcd.print("HELLO WORLD!");
  lcd.setCursor(0, 1);
  lcd.print("PROJEK kita");
  lcd.setCursor(14, 1);
  lcd.print(hitung);
  delay(1000);
  lcd.clear();
}
