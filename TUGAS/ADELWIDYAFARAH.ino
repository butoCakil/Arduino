#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 

void setup() {
lcd.begin(16,2);
lcd.init();
lcd.backlight();
}

void loop() {
lcd.begin(16,2);
lcd.init();
lcd.backlight();
lcd.setCursor(3, 0);
lcd.print(" SMKBANSARI");

lcd.setCursor(2, 1);
lcd.print("T.ELEKTRONIK");
delay(3000);
lcd.clear();

lcd.setCursor(3, 0);

lcd.print(" SMKBOS");

lcd.setCursor(1, 1);
lcd.print("Bansariorasepele");
delay(3000);
lcd.clear();

lcd.setCursor(3,0);
lcd.print("Adel");

lcd.setCursor(3,1);
lcd.print("XTE2");
delay(3000);
lcd.clear();

lcd.setCursor(2, 0);
lcd.print("Widya");

lcd.setCursor(3,1);
lcd.print("XTE3");
delay(3000);
lcd.clear();

lcd.setCursor(4, 0);
lcd.print("Farah");

lcd.setCursor(3,1);
lcd.print("XTE1");
delay(3000);
lcd.clear();
}

