// -----------------------------------------------
// Running Text untuk menampilkan tulisan yang panjang
// dan berkesan tergulir
// menggunakan LCD dengan modul I2C
// ----------------------------------------------

#include <LiquidCrystal_I2C.h> // Memasukkan library untuk I2C 
LiquidCrystal_I2C lcd(0x3F, 16, 2); // Alamat default I2C = 0x27 dgn LCD 16x2

// Definisikan string 
String teks = "Kosong Adalah isi, isi adalah kosong.";

int indeks = 0;

void setup()
{
 lcd.init(); // inisialisasi lcd 
 lcd.backlight(); // Menyalakan lampu belakang

// letakan tambahan disini
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

  // selesai

 lcd.clear();
 lcd.print("Tom Sam Chong:"); 

 // Bentuk string yang panjang
//  teks = teks + "Cobalah semua contoh tulis sesukamu, yang penting panjang";
}

void loop()
{
 char st[17]; // String dengan maksimal 16 karakter

 st[16] = 0; // Akhir string

 // Bentuk string
 for (int j = 0; j < 16; j++)
    if (indeks + j < teks.length())
        st[j] = teks[indeks + j];
    else
        st[j] = 32; // Spasi

 // Tampilkan di baris kedua
 lcd.setCursor(0, 1);
 lcd.print(st);

 // Peroleh indeks berikutnya
 indeks++;
 
 if (indeks == teks.length() - 1)
 indeks = 0;
 
 delay(600);
}
