 #include <LiquidCrystal_I2C.h>
 LiquidCrystal_I2C lcd(0x27, 16, 2);

String teks = "KATANYA MAU PINTER KOK MALAS BELAJAR ";

int indeks = 0;

void setup()
{
  lcd.init();
  lcd.backlight();
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
  lcd.print("X TE 3");


  delay(2000);
  lcd.clear();

  lcd.print(" PERHATIKAN INI: ");

  teks = teks + "AYOO SEMANGAT BELAJARNYA";
 }
void loop()
{
  char st[17];
  st[16] = 0;


  for(int j = 0; j < 16; j++)
    if (indeks + j < teks.length())
       st[j] = teks[indeks + j];
   else
       st[j] = 32;


 lcd.setCursor(0, 1);
 lcd.print(st);


 indeks++;

 if (indeks == teks.length() - 1)
 indeks = 0;

 delay(1000);
}
