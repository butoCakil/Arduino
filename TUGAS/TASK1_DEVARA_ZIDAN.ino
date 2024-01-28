#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup() {
  // put your setup code here, to run once:
 
}
void loop() {
  // put your main code here, to run repeatedly:
  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2,0);
  lcd.print("SMKN BANSARI");
  lcd.setCursor(1,1);
  lcd.print("T.ELEKTRONIKA");
  delay(4000);
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("SMK BOS");
  lcd.setCursor(0,1);
  lcd.print("BANSARIORASEPELE");
  delay(4000);
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("DEVARA");
  lcd.setCursor(5,1);
  lcd.print("X TE 1");
  delay(4000);
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("ZIDAN");
  lcd.setCursor(5,1);
  lcd.print("X TE 2");
  delay(4000);
  lcd.clear();
}
