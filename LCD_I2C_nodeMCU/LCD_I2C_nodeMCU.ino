#include <LiquidCrystal_I2C.h>
 
// Konstruk object LCD dengan alamat I2C
// Ganti 0x3F sesuai dengan alamat I2C modul kalian
// Jika tidak tahu dapat menggunakan LCD I2C Scanner
// LiquidCrystal_I2C lcd(0x3F, 16, 2);
LiquidCrystal_I2C lcd(0x27, 16, 2); // 16x2
 
void setup() {
  // Pemanggilan pertama memerlukan parameter jumlah kolom dan baris
  // Ini harus sama dengan yang dimasukan pada konstruktor.
  lcd.begin(16,2);
  lcd.init();
 
  // Nyalakan backlight
  lcd.backlight();
 
  // Pindahkan kursor ke kolom 0 dan baris 0
  // (baris 1)

  // x, y
  
}
 
void loop() {
  //Kode loop silahkan disesuaikan
  //...

  lcd.setCursor(0, 0);
 
  // Cetak hellow ke layar
  lcd.print("HELLO WORLD!");
 
  // Pindahkan kursor ke baris berikutnya dan cetak lagi
  lcd.setCursor(0, 1);      
  lcd.print("PROJEK kita");

  delay(2000);

  lcd.clear();

  lcd.setCursor(5, 0);
  lcd.print("***");

  delay(2000);
  lcd.clear();
}
