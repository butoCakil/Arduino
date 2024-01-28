  #include <LiquidCrystal_I2C.h>
  
  LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.begin(16,2);
  lcd.init();

  lcd.backlight();


}
  
void loop() {
  lcd.setCursor(2, 0);

  lcd.print("SMKN BANSARI");
  
 lcd.setCursor(1, 1);
  lcd.print ("T.ELEKTRONIKA");

  delay(2000);
  lcd.clear();
  
  lcd.setCursor(4, 0);
  lcd.print("SMK BOS");
  
  lcd.setCursor(0, 1);
  lcd.print("BANSARIORASEPELE");

  delay(2000);
  lcd.clear();

  lcd.setCursor(5, 0);
  lcd.print("FATIKA");

  lcd.setCursor(5, 1);
  lcd.print("X TE 2");

  delay(2000);
  lcd.clear();

  lcd.setCursor(4, 0);
  lcd.print("RAHMANIA");

  lcd.setCursor(5, 1);
  lcd.print("X TE 2");


  delay(2000);
  lcd.clear();

 lcd.setCursor(6, 0);
  lcd.print("PUTRI");

  lcd.setCursor(5, 1);
  lcd.print("X TE 2");


  delay(2000);
  lcd.clear();
 }
 
 
