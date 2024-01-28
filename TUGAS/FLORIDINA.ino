#include <LiquidCrystal_I2C.h> // memasukan library untuk I2C 
LiquidCrystal_I2C lcd(0x3F, 16, 2);// alamat default I2C = 0x27 dgn LCO 16X2

// Definisikan string
String teks = " SEHAT-SEHAT BADAN,EPISODE KITA MASIH PANJANG :). ";

int indeks = 0;

void setup() 
{
  lcd.init();// inisialisasi lcd
  lcd.backlight();//menyalakan lampu belakang

//disini
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
//sampesini
  
  lcd.clear();
  lcd.print("FAREL GANTENG:");

  // Bentuk string yang panjang 
  teks = teks + "LEVEL TERTINGGI MENCITAI ADALAH KETIKA BERADA DI FASE A LITTLE PIECE OF HEAVEN :)";
}

void loop() 
{
char st[17]; // string dengan maksimal 16 karakter


st[16] =0; // Akhir string 

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

if(indeks == teks.length() - 1)
indeks = 0;

delay(600);
}
