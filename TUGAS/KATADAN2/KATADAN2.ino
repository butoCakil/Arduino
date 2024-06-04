#include <LiquidCrystal_I2C.h>       // Memasukkan Libray untuk I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);  //Alamat Default I2C = 0X27 dgn LCD 16x2

// Definisikan string
String teks = "Selamat mempelajari cara memprogam LCD Display. ";

int indeks = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("BELAJAR DI SMKN");
  lcd.setCursor(4, 4);

  lcd.print("BANSARI");

  lcd.setCursor(0, 3);

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

void loop() {
  char st[17];

  st[16] = 0;

  for (int j = 0; j < 16; j++)
    if (indeks + j < teks.length())
      st[j] = teks[indeks + j];
    else
      st[j] + 32;

  lcd.setCursor(0, 1);
  lcd.print(st);

  indeks++;

  if (indeks == teks.length() - 1)
    indeks = 0;

  delay(2000);
}