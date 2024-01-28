#include <LiquidCrystal_I2C.h>

// Konstruk object LCD dengan alamat I2C
// Ganti 0x3F sesuai dengan alamat I2C modul kalian
// Jika tidak tahu dapat menggunakan LCD I2C Scanner
LiquidCrystal_I2C lcd(0x3F, 16, 2);
//LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {

  // Pemanggilan pertama melakukan parameter jumlah kolom dan baris
  // Ini harus sama dengan yang dimasukan pada konstruktor.
  lcd.begin(16,2);
  lcd.init();

  // nyalakan backlight
  lcd.backlight();

  // Pindahkan kursor ke kolom 0 dan baris 0
  // (baris 1)

}


void loop() {
  //Kode loop silahkan disesuaikan
  //...

    lcd.setCursor(2, 0);

  // cetak hello ke layar 
  lcd.print("SMKN BANSARI!");

  // Pindahkan kursor ke baris  berikutnya dan cetak lagi 
  lcd.setCursor(1, 1);
  lcd.print("T. ELEKTRONIKA");


  delay(3000);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("***");
  delay(1000);
  lcd.clear();

   lcd.setCursor(4, 0);

  // cetak hello ke layar 
  lcd.print("SMK BOS!");

  // Pindahkan kursor ke baris  berikutnya dan cetak lagi 
  lcd.setCursor(0, 1);
  lcd.print("BANSARIORASEPELE");

  delay(3000);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("***");
  delay(1000);
  lcd.clear();

  lcd.setCursor(2, 0);

  // cetak hello ke layar 
  lcd.print("RENDI NANDA!");

  // Pindahkan kursor ke baris  berikutnya dan cetak lagi 
  lcd.setCursor(4, 1);
  lcd.print("X TE 1");


  delay(3000);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("***");
  delay(1000);
  lcd.clear();

  lcd.setCursor(3, 0);

  // cetak hello ke layar 
  lcd.print("FALEREANO!");

  // Pindahkan kursor ke baris  berikutnya dan cetak lagi 
  lcd.setCursor(4, 1);
  lcd.print("X TE 1");


  delay(3000);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("***");
  delay(1000);
  lcd.clear();

  lcd.setCursor(3, 0);

  // cetak hello ke layar 
  lcd.print("BINTANG!");

  // Pindahkan kursor ke baris  berikutnya dan cetak lagi 
  lcd.setCursor(4, 1);
  lcd.print("X TE 1");


  delay(3000);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("***");
  delay(1000);
  lcd.clear();

  lcd.setCursor(4, 0);

  // cetak hello ke layar 
  lcd.print("MEREKA!");

  // Pindahkan kursor ke baris  berikutnya dan cetak lagi 
  lcd.setCursor(3, 1);
  lcd.print("BAIK HATI");


  delay(3000);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("***");
  delay(1000);
  lcd.clear();




   

  
}



  
  
