#include <LiquidCrystal_I2C.h>      
LiquidCrystal_I2C lcd(0x27, 16, 2);  

String teks = "Selamat mempelajari cara memprogram LCD display. ";
int indeks = 0;

void setup() 
{
  lcd.init();
  lcd.backlight();
lcd.setCursor(2,0);

lcd.print("SMKN BANSARI");
  
  lcd.setCursor(1,1);
lcd.print("T.ELEKTRONIKA");

lcd.setCursor(4,0);
lcd.print("SMK BOS");

lcd.setCursor(0,1);
lcd.print("BANSARORASEPELE");

delay(2000);
lcd.clear();

lcd.setCursor(5,0);
lcd.print("FARAH");

lcd.setCursor(5,1);
lcd.print("XTE1");

delay(2000);
lcd.clear();

lcd.setCursor(4,0);
lcd.print("WIDYA");

lcd.setCursor(5,1);
lcd.print("XTE3");

delay(2000);
lcd.clear();

lcd.setCursor(6,0);
lcd.print("SMKN BANSARI");

lcd.setCursor(5,1);
lcd.print("SMKN BANSARI");

delay(2000);
lcd.clear();

lcd.print("PERHATIKAN INI:");

  teks = teks + "Modul yang dipakai untuk menerangi penggunaan kaki di LCD.";
}

void loop() 
{
  char st[17];
  st[16] = 0;  

  
  for (int j = 0; j < 16; j++)
    if (indeks + j < teks.length())
      st[j] = teks[indeks = j];
    else
      st[j] = 32;  

  
  lcd.setCursor(0, 1);
  lcd.print(st);

  
  indeks++;

  if (indeks == teks.length() - 1)
    indeks = 0;

  delay(1000);
}
