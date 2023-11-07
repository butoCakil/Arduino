// -----------------------------------------------
// Running Text untuk menampilkan tulisan yang panjang
// dan berkesan tergulir
// menggunakan LCD dengan modul I2C
// Oleh : Hendry Kurniawan
// ----------------------------------------------

#include <LiquidCrystal_I2C.h> // Memasukkan library untuk I2C 
LiquidCrystal_I2C lcd(0x3F, 16, 2); // Alamat default I2C = 0x27 dgn LCD 16x2

// Definisikan string 
String teks = "Selamat mempelajari cara memprogram LCD Display. ";

int indeks = 0;

void setup()
{
 lcd.init(); // inisialisasi lcd 
 lcd.backlight(); // Menyalakan lampu belakang
 lcd.clear();
 lcd.print("Info untuk Anda:"); 

 // Bentuk string yang panjang
 teks = teks + "Cobalah semua contoh di www.elektronikahendry.com - " + "Kemudian, Anda bisa mengembangkannya sendiri " + "sesuai dengan ide Anda. Sekali lagi, selamat belajar " + "dan sukses selalu!";
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
