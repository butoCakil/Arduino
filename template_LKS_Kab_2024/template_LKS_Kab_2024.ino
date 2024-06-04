#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <SPI.h>

#define SDA_PIN 10
#define RST_PIN 9

#define BUZZER 8
#define RELAY 6

MFRC522 mfrc522(SDA_PIN, RST_PIN);

// LiquidCrystal_I2C lcd(0x3F, 16, 2);
LiquidCrystal_I2C lcd(0x27, 16, 2);

char nokartu[20];
char idKunci[20] = "BA7B73D4";
bool bukaKunci = false;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  mfrc522.PCD_Init();

  pinMode(BUZZER, OUTPUT);
  pinMode(RELAY, OUTPUT);

  digitalWrite(BUZZER, LOW);
  digitalWrite(RELAY, HIGH);

  Serial.println("Baca Kartu RFID");
  beep(2);
}

void loop() {
  if (bacaKartu()) {
    beep(1);
    Serial.print("ID Kartu: ");
    Serial.println(nokartu);

    if (strcmp(idKunci, nokartu) == 0) {
      statusKunci(true);
      Serial.print("ID SESUAI");
      beep(1);
    } else {
      beep(3);
      statusKunci(false);
      Serial.print("ID SALAH");
      lcd.setCursor(0, 1);
      lcd.print("XXXX KARTU SALAH");

      delay(1000);
    }
  }

  // lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("SMART LOCK DOOR");

  if (bukaKunci) {
    Serial.println(">>> Terbuka  <<<");

    lcd.setCursor(0, 1);
    lcd.print(">>> Terbuka  <<<");
  } else {
    Serial.println("XXX Terkunci XXX");

    lcd.setCursor(0, 1);
    lcd.print("XXX Terkunci XXX");
  }

  if (bukaKunci)
    digitalWrite(RELAY, LOW);
  else
    digitalWrite(RELAY, HIGH);

  delay(1000);
}

int bacaKartu() {
  if (!mfrc522.PICC_IsNewCardPresent())
    return 0;

  if (!mfrc522.PICC_ReadCardSerial())
    return 0;

  memset(nokartu, 0, sizeof(nokartu));

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    snprintf(nokartu, sizeof(nokartu), "%s%02X", nokartu, mfrc522.uid.uidByte[i]);
  }

  Serial.print("Nokartu: ");
  Serial.println(nokartu);

  return 1;
}

void statusKunci(bool _status) {
  if (_status)
    bukaKunci = !bukaKunci;
}

void beep(int jmlbeep) {
  for (int i = 0; i < jmlbeep; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);
    delay(100);
  }
}