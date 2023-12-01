#include <Arduino.h>
#include <TM1637Display.h>
#include <Ticker.h>

Ticker timer;

int runDetik = 15;
int runMenit = 1;
int runJam = 0;

int kedipDot;

#define BUTTON_PIN 5  //D1
boolean lastButtonState = HIGH;
boolean buttonState = HIGH;

#define RESET_BUTTON_PIN 4  //D2
boolean lastResetButtonState = HIGH;
boolean resetButtonState = HIGH;

boolean dot = false;
boolean timerBerjalan = false;
boolean timerBerhenti = false;
boolean tampilanStop = false;

unsigned long timeLeft;
unsigned long countdownDuration = runJam * (60 * 60) + (runMenit * 60) + (runDetik);
unsigned long startTime;
unsigned long pauseTime;
unsigned long waktuSebelum = 0;
unsigned long waktuDetikSebelum = 0;
unsigned long waktuGanti1 = 0;
unsigned long waktuCoundown = 0;

#define TEST_DELAY 2000
#define SATU_DETIK 1000
#define STGH_DETIK 500
#define FLASH 100

#define CLK 0  //D3
#define DIO 2  //D4

TM1637Display display(CLK, DIO);

const uint8_t SEG_STRT[] = {
  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,  // S
  SEG_F | SEG_G | SEG_E | SEG_D,          // t
  SEG_E | SEG_G,                          // r
  SEG_F | SEG_G | SEG_E | SEG_D,          // t
};

const uint8_t SEG_STOP[] = {
  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,          // S
  SEG_F | SEG_G | SEG_E | SEG_D,                  // t
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
  SEG_A | SEG_B | SEG_F | SEG_G | SEG_E,          // t
};

void setup() {
  Serial.begin(115200);
  Serial.println("Menghitung mundur selama 2 menit...");

  delay(1000);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  pinMode(CLK, OUTPUT);
  pinMode(DIO, OUTPUT);

  // Catat waktu mulai
  startTime = millis();

  display.setBrightness(0x0f);
  display.clear();

  if (runJam > 0) {
    tampilTimer(runJam, runMenit);
  } else {
    tampilTimer(runMenit, runDetik);
  }

  delay(3000);
}

void loop() {
  bacaTombol();

  while (timerBerhenti == true) {
    Serial.print("(1) timerBerhenti: ");
    Serial.println(timerBerhenti);

    // Setiap detik, beralih antara SEG_STOP dan timer
    if (millis() - waktuGanti1 >= SATU_DETIK) {
      waktuGanti1 = millis();

      Serial.println("(2) Ganti Tampilan STOP");

      // Ganti tampilan setiap detik
      if (tampilanStop) {
        Serial.println("(3) STOP");
        display.setSegments(SEG_STOP);
        tampilanStop = false;
        timerBerhenti = true;
      } else {
        Serial.println("(4) 00:00");
        tampilTimer(0, 0);
        tampilanStop = true;
        timerBerhenti = true;
      }
    }

    bacaTombol();
  }
}

void bacaTombol() {
  // Baca status tombol
  buttonState = digitalRead(BUTTON_PIN);

  // Deteksi perubahan pada tombol
  if (buttonState != lastButtonState) {
    // Tunggu hingga tombol stabil
    delay(50);

    // Ubah status timerBerjalan berdasarkan tombol
    if (buttonState == LOW) {
      Serial.println("(5) Tombol Set Ditekan");
      Serial.println(countdownDuration);
      timerBerjalan = !timerBerjalan;

      // Reset waktu mulai jika timer dimulai kembali
      if (timerBerjalan) {
        Serial.print("(6) Timer Berjalan lagi: ");
        Serial.println(countdownDuration);
        startTime = millis() - pauseTime;
        // timer.attach(1, timerCallback);  // Mendaftarkan kembali callback timer
        timer.attach_ms(100, timerCallback);
      }
    }
  }

  // Baca status tombol reset
  resetButtonState = digitalRead(RESET_BUTTON_PIN);

  // Deteksi perubahan pada tombol reset
  if (resetButtonState != lastResetButtonState) {
    // Tunggu hingga tombol reset stabil
    delay(50);

    // Reset timer jika tombol reset ditekan saat timer berhenti
    if (!timerBerjalan && resetButtonState == LOW) {
      Serial.println("(7) Tombol RESET ditekan: ");
      timerBerhenti = false;
      startTime = millis();
      pauseTime = 0;

      countdownDuration = runJam * 3600 + runMenit * 60 + runDetik;

      if (runJam > 0) {
        tampilTimer(runJam, runMenit);
      } else {
        tampilTimer(runMenit, runDetik);
      }

      Serial.print("(8) countdownDuration");
      Serial.println(countdownDuration);
      timeLeft = countdownDuration;
      printTime(timeLeft);
    }
  }

  // Simpan status tombol dan resetButton
  lastButtonState = buttonState;
  lastResetButtonState = resetButtonState;
}

void timerCallback() {
  if (timerBerjalan) {
    Serial.println("(9) Timer Berjalan");
    if (millis() - waktuCoundown >= 1000) {
      // Hitung waktu yang telah berlalu
      unsigned long elapsedTime = millis() - startTime;
      pauseTime = millis() - startTime;

      Serial.print("(10) elapsedTime");
      Serial.println(elapsedTime);

      // Hitung mundur tersisa
      timeLeft = countdownDuration - (elapsedTime / 1000);

      Serial.print("(11) timeleft: ");
      Serial.println(timeLeft);

      // Tampilkan informasi di Serial Monitor
      printTime(timeLeft);

      // Periksa apakah hitung mundur telah selesai
      if (elapsedTime >= countdownDuration * 1000) {
        timerBerjalan = false;
        timerBerhenti = true;

        Serial.print("(12) elapsedTime: ");
        Serial.println(elapsedTime);
        Serial.print("(13) countdownDuration: ");
        Serial.println(countdownDuration);
        timer.detach();  // Matikan timer setelah hitung mundur selesai
        display.clear();
      }

      waktuCoundown = millis();
    } else {
      printTime(timeLeft);
    }
  } else {
    printTime(timeLeft);
  }
}

void printTime(unsigned long seconds) {
  // Hitung jam, menit, dan detik
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int secs = seconds % 60;

  // Tampilkan waktu di Serial Monitor
  // Serial.print(hours);
  // Serial.print(":");
  // if (minutes < 10) Serial.print("0");  // Tambahkan nol di depan jika menit kurang dari 10
  // Serial.print(minutes);
  // Serial.print(":");
  // if (secs < 10) Serial.print("0");  // Tambahkan nol di depan jika detik kurang dari 10
  // Serial.println(secs);

  if (hours > 0) {
    tampilTimer(hours, minutes);
  } else {
    tampilTimer(minutes, secs);
  }
}

void tampilTimer(int angkaAwal, int angkaAkhir) {
  unsigned long jamTimer = (angkaAwal * 100) + angkaAkhir;

  Serial.print("(14) jamTimer: ");
  Serial.println(jamTimer);

  kedipDot = FLASH;
  blinkDot(jamTimer, true);
}

void blinkDot(int getTime, boolean status) {
  if (timerBerjalan) {
    unsigned long waktuSekarang = millis();

    if (waktuSekarang - waktuSebelum >= kedipDot) {
      if (dot == true) {
        dot = false;
        display.showNumberDecEx(getTime, 0b11000000, status);
      } else {
        dot = true;
        display.showNumberDecEx(getTime, 0b00000000, status);
      }

      waktuSebelum = millis();
    }
  } else {
    display.showNumberDecEx(getTime, 0b11000000, status);
  }
}
