#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
String teks = "Selamat mempelajari cara memprogram LCD Display. ";
int indeks = 0;
void setup() 
{
  // put your setup code here, to run once:
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
void loop() 
{
  // put your main code here, to run repeatedly:
  lcd.init();
  lcd.backlight();
  lcd.print("DEVARA ZIDAN");
  teks = teks + "Gelas kopi ditangan kiri dan tinta ditangan kanan selembar kertas diatas meja" + "Tak ada yang bisa kau tulis selain kerinduan tentang dia" + "Bermimpilah setinngi langit jika jatuh kau akan jatuh diantara bintang bintang!";
char st[17];
st[16] = 0;
for (int j = 0; j < 16; j++)
if (indeks + j < teks.length())
st[j] = teks[indeks + j];
else
st[j] = 32;
lcd.setCursor(0,1);
lcd.print(st);
indeks++;
if (indeks == teks.length() - 1)
indeks = 0;
delay(600);
}
