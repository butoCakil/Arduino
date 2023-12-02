#include <Arduino.h>
#include <TM1637Display.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WiFiClient.h>
#include "login.h"
#include "home.h"
#include "config.h"
#include "error.h"
#include "about.h"
#include <EEPROM.h>

Ticker timer;

int runDetik = 0;
int runMenit = 0;
int runJam = 0;

int kedipDot;

String StringJam = String(runJam < 10 ? "0" + String(runJam) : String(runJam)) + ":" + String(runMenit < 10 ? "0" + String(runMenit) : String(runMenit)) + ":" + String(runDetik < 10 ? "0" + String(runDetik) : String(runDetik));

#define BUTTON_PIN 5  //D1
boolean lastButtonState = HIGH;
boolean buttonState = HIGH;

#define RESET_BUTTON_PIN 4  //D2
boolean lastResetButtonState = HIGH;
boolean resetButtonState = HIGH;

#define BUZZER 14  //D5
#define LOAD 12    //D6

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

char chipID[25];

TM1637Display display(CLK, DIO);

String ssidNew, passNew, hostNew;
String usernameLogin, passwordLogin;
String setJam, setMenit, setDetik;

AsyncWebServer server(80);

// EEPROM Konfigurasi
const int eepromSize = 512;
const int debounceDelay = 50;

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

// function handel EEPROM dan page Akses poin
void writeStringToEEPROM(int startAddr, String data) {
  EEPROM.begin(eepromSize);
  for (int i = 0; i < data.length(); i++) {
    EEPROM.write(startAddr + i, data[i]);
  }
  EEPROM.write(startAddr + data.length(), '\0');  // Null-terminate the string
  EEPROM.commit();
  EEPROM.end();
}

String readStringFromEEPROM(int startAddr) {
  EEPROM.begin(eepromSize);
  String data = "";
  char ch;
  do {
    ch = EEPROM.read(startAddr + data.length());
    data += ch;
  } while (ch != '\0');
  EEPROM.end();
  return data.substring(0, data.length() - 1);  // Remove the null character
}

void handleRoot(AsyncWebServerRequest *request) {
  String formattedHtml = String(login_html);
  formattedHtml.replace("%s", chipID);
  formattedHtml.replace("%c", "/");
  request->send(200, "text/html", formattedHtml);
}

void handleConfig(AsyncWebServerRequest *request) {
  String formattedHtml = String(config_html);
  formattedHtml.replace("%alert%", "");
  formattedHtml.replace("%href%", String("/home?run=false&username=") + usernameLogin + "&password=" + passwordLogin);
  request->send(200, "text/html", formattedHtml);
}

void handleSaveConfig(AsyncWebServerRequest *request) {
  runJam = request->arg("setJam").toInt();
  runMenit = request->arg("setMenit").toInt();
  runDetik = request->arg("setDetik").toInt();

  writeStringToEEPROM(0, String(runJam));
  writeStringToEEPROM(64, String(runMenit));
  writeStringToEEPROM(128, String(runDetik));

  StringJam = String(runJam < 10 ? "0" + String(runJam) : String(runJam)) + ":" + String(runMenit < 10 ? "0" + String(runMenit) : String(runMenit)) + ":" + String(runDetik < 10 ? "0" + String(runDetik) : String(runDetik));

  display.clear();

  timerBerhenti = false;
  startTime = millis();
  pauseTime = 0;

  countdownDuration = runJam * 3600 + runMenit * 60 + runDetik;

  if (runJam > 0) {
    tampilTimer(runJam, runMenit);
  } else {
    tampilTimer(runMenit, runDetik);
  }

  timeLeft = countdownDuration;
  printTime(timeLeft);

  String alert = "<div id=\"alert\"><p>Berhasil Simpan</p></div>";
  String formattedHtml = String(config_html);
  formattedHtml.replace("%alert%", alert);
  formattedHtml.replace("%href%", String("/home?run=false&username=") + usernameLogin + "&password=" + passwordLogin);
  request->send(200, "text/html", formattedHtml);
}

void handleNotFound(AsyncWebServerRequest *request) {
  String formattedHtml = String(error_html);
  formattedHtml.replace("%s", "404 - Halaman Tidak ditemukan");
  formattedHtml.replace("%c", "/");
  request->send(404, "text/html", formattedHtml);
}

void handleReboot(AsyncWebServerRequest *request) {
  request->send(200, "text/html", selesai_html);
  delay(5000);
  ESP.restart();
}

void handleHome(AsyncWebServerRequest *request) {
  boolean usernameOK = false;
  boolean passwordOK = false;

  usernameLogin = request->arg("username");
  passwordLogin = request->arg("password");

  String runParam = request->arg("run");

  if (runParam == "true") {
    timerBerjalan = true;
    playPauseTimer();
  } else if (runParam == "false") {
    timerBerjalan = false;
    playPauseTimer();
  } else if (runParam == "reset") {
    if (timerBerjalan == false) {
      resetTimer();
    }
  }

  if (usernameLogin == "admin") {
    usernameOK = true;
  }

  if (passwordLogin == "admin$123") {
    passwordOK = true;
  }

  if (usernameOK && passwordOK) {
    String statusTimer = "";

    if (timerBerjalan == true) {
      statusTimer = "STOP";
    } else {
      statusTimer = "START";
    }

    String formattedHtml = String(home_html);
    formattedHtml.replace("%His%", StringJam);
    formattedHtml.replace("%START%", statusTimer);
    formattedHtml.replace("%setjam%", String(runJam));
    formattedHtml.replace("%setmenit%", String(runMenit));
    formattedHtml.replace("%setdetik%", String(runDetik));
    formattedHtml.replace("%href%", "/");
    request->send(200, "text/html", formattedHtml);
  } else if (!usernameOK && passwordOK) {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "Login Gagal - Username Login Salah!");
    formattedHtml.replace("%c", "/");
    request->send(404, "text/html", formattedHtml);
  } else if (usernameOK && !passwordOK) {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "Login Gagal - Password Login Salah!");
    formattedHtml.replace("%c", "/");
    request->send(404, "text/html", formattedHtml);
  } else if (usernameLogin == "" || passwordLogin == "") {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "Login Gagal - Login dulu ya..");
    formattedHtml.replace("%c", "/");
    request->send(404, "text/html", formattedHtml);
  } else {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "Login Gagal - Username dan Password Login Salah!");
    formattedHtml.replace("%c", "/");
    request->send(404, "text/html", formattedHtml);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Menghitung mundur selama 2 menit...");

  delay(1000);

  // EEPROM config
  setJam = readStringFromEEPROM(0);
  setMenit = readStringFromEEPROM(64);
  setDetik = readStringFromEEPROM(128);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  pinMode(LOAD, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DIO, OUTPUT);

  digitalWrite(BUZZER, LOW);
  digitalWrite(LOAD, LOW);

  // Get ESP8266 Chip ID
  int num = ESP.getChipId();
  itoa(num, chipID, 10);

  // SET data dari EEPROM
  runJam = setJam.toInt();
  runMenit = setMenit.toInt();
  runDetik = setDetik.toInt();

  StringJam = String(runJam < 10 ? "0" + String(runJam) : String(runJam)) + ":" + String(runMenit < 10 ? "0" + String(runMenit) : String(runMenit)) + ":" + String(runDetik < 10 ? "0" + String(runDetik) : String(runDetik));

  countdownDuration = runJam * (60 * 60) + (runMenit * 60) + (runDetik);

  // MOde AP
  String ssidString = "TIMER-" + String(chipID);
  const char *_ssid = ssidString.c_str();
  const char *_password = "admin$123";

  WiFi.mode(WIFI_AP);
  WiFi.softAP(_ssid, _password);
  server.begin();  // Mulai server dalam mode Akses Poin

  IPAddress IP = WiFi.softAPIP();
  Serial.println("");
  Serial.print("AP: ");
  Serial.println(_ssid);
  Serial.print("Password: ");
  Serial.println(_password);
  Serial.print("IP Address (AP): ");
  Serial.println(IP);

  delay(1000);

  server.on("/getStringJam", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", StringJam.c_str());
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    handleRoot(request);
  });

  server.on("/home", HTTP_GET, [](AsyncWebServerRequest *request) {
    handleHome(request);
  });

  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request) {
    handleConfig(request);
  });

  server.on("/setting", HTTP_GET, [](AsyncWebServerRequest *request) {
    handleSaveConfig(request);
  });

  server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request) {
    handleReboot(request);
  });

  server.onNotFound([](AsyncWebServerRequest *request) {
    handleNotFound(request);
  });


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
  stopStatusTimer();

  // if (!timerBerhenti && timerBerjalan) {
  if (timer.active()) {
    digitalWrite(LOAD, HIGH);
  } else {
    digitalWrite(LOAD, LOW);
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
      timerBerjalan = !timerBerjalan;

      playPauseTimer();
    }
  }

  // Baca status tombol reset
  resetButtonState = digitalRead(RESET_BUTTON_PIN);

  // Deteksi perubahan pada tombol reset
  if (resetButtonState != lastResetButtonState) {
    // Tunggu hingga tombol reset stabil
    delay(50);

    // Reset timer jika tombol reset ditekan saat timer berhenti
    if (resetButtonState == LOW) {
      resetTimer();
    }
  }

  // Simpan status tombol dan resetButton
  lastButtonState = buttonState;
  lastResetButtonState = resetButtonState;
}

void stopStatusTimer() {
  buz();

  if (timerBerhenti == true) {
    // Setiap detik, beralih antara SEG_STOP dan timer
    if (millis() - waktuGanti1 >= SATU_DETIK) {
      waktuGanti1 = millis();

      // Ganti tampilan setiap detik
      if (tampilanStop) {
        display.setSegments(SEG_STOP);
        tampilanStop = false;
      } else {
        tampilTimer(0, 0);
        tampilanStop = true;
      }

      timerBerhenti = true;
      timerBerjalan = false;
    }

    bacaTombol();
  }
}

void buz() {
  if (timerBerhenti) {
    if (tampilanStop) {
      digitalWrite(BUZZER, HIGH);
    } else {
      digitalWrite(BUZZER, LOW);
    }
  }
}

void buzzerON(int _loop) {
  for (int i = 0; i < _loop; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);
    delay(100);
  }
}

void playPauseTimer() {
  // Reset waktu mulai jika timer dimulai kembali
  if (timerBerjalan) {
    startTime = millis() - pauseTime;
    timer.attach_ms(100, timerCallback);
  }
}

void resetTimer() {
  if (!timerBerjalan) {
    digitalWrite(BUZZER, LOW);
    
    timer.detach();
    display.clear();

    timerBerhenti = false;
    startTime = millis();
    pauseTime = 0;

    countdownDuration = runJam * 3600 + runMenit * 60 + runDetik;

    if (runJam > 0) {
      tampilTimer(runJam, runMenit);
    } else {
      tampilTimer(runMenit, runDetik);
    }

    timeLeft = countdownDuration;
    printTime(timeLeft);


  }
}

void timerCallback() {
  if (timerBerjalan) {
    if (millis() - waktuCoundown >= 1000) {
      // Hitung waktu yang telah berlalu
      unsigned long elapsedTime = millis() - startTime;
      pauseTime = millis() - startTime;

      // Hitung mundur tersisa
      timeLeft = countdownDuration - (elapsedTime / 1000);

      // Tampilkan informasi di Serial Monitor
      printTime(timeLeft);

      // Periksa apakah hitung mundur telah selesai
      if (elapsedTime >= countdownDuration * 1000) {
        timerBerjalan = false;
        timerBerhenti = true;

        timer.detach();  // Matikan timer setelah hitung mundur selesai
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

  StringJam = String(hours < 10 ? "0" + String(hours) : String(hours)) + ":" + String(minutes < 10 ? "0" + String(minutes) : String(minutes)) + ":" + String(secs < 10 ? "0" + String(secs) : String(secs));

  if (hours > 0) {
    tampilTimer(hours, minutes);
  } else {
    tampilTimer(minutes, secs);
  }
}

void tampilTimer(int angkaAwal, int angkaAkhir) {
  unsigned long jamTimer = (angkaAwal * 100) + angkaAkhir;

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
