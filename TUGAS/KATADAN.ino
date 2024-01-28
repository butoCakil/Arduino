#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.begin(16,2);
  lcd.init();

  lcd.backlight();

  
}

void loop() {
  lcd.setCursor(3, 0);
  lcd.print("SMK N BANSARI");

  lcd.setCursor(0, 3);
  lcd.print("T. ELEKTRONIKA");

  delay(2000);

  lcd.clear();
  //
  lcd.setCursor(3, 0);
  lcd.print("SMK BOS");
  
  lcd.setCursor(0, 3);
  lcd.print("BansariOraSepele");

  delay(2000);
  
  lcd.clear();
  //
  lcd.setCursor(1, 0);
  lcd.print("M.Azka Alkaesa");

  lcd.setCursor(0, 3);
  lcd.print("X TE 3");

  delay(2000);

  lcd.clear();
  //
  lcd.setCursor(3, 0);
  lcd.print("Desta .R");

  lcd.setCursor(0, 3);
  lcd.print("X TE 2");

  delay(2000);

  lcd.clear();
  //
  lcd.setCursor(1, 0);
  lcd.print("Fardhan Azmi H");

  lcd.setCursor(0, 3);
  lcd.print("X TE 2");

  delay(2000);

  lcd.clear();


}