#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "index.h"
#include "login.h"
#include "error.h"
#include <EEPROM.h>
#include <MFRC522.h>

#include <U8g2lib.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define buttonPin 2
const int eepromSize = 512;
const int debounceDelay = 50;

unsigned long buttonPressTime = 0;
unsigned long lastTimeConnected = 0;
boolean tombolDitekan = false;
boolean modeAP = false;
boolean tungguRespon = false;

String ssidNew = "", passNew, nodeviceNew, hostNew;
String usernameLogin, passwordLogin;

String chipID;

String formattedHtml = "Chip ID: %IDCHIP%";

char nodevice[20] = "";
char IDTAG[20];

AsyncWebServer server(80);

#define SDA_PIN 5
#define RST_PIN 27
MFRC522 mfrc522(SDA_PIN, RST_PIN);

// Define NTP Client to get time
const long utcOffsetInSeconds = 25200;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "id.pool.ntp.org", utcOffsetInSeconds);
String weekDays[7] = { "Min", "Sen", "Sel", "Rab", "Kam", "Jum", "Sab" };
String months[12] = { "Jan", "Feb", "Mar", "Apr", "Mei", "Jun", "Jul", "Agt", "Sep", "Okt", "Nov", "Des" };

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);  //Software I2C

unsigned long startTimeBootLoad;

int nom;

int screenWidth = u8g2.getWidth();
int screenHeight = u8g2.getHeight();


// 'check-3x', 24x24px
const unsigned char epd_bitmap_check_3x[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x1c, 0x00,
  0x00, 0x3e, 0x00, 0x00, 0x7f, 0x00, 0x80, 0x7f, 0x00, 0xc0, 0x3f, 0x10, 0xe0, 0x1f, 0x38, 0xf0,
  0x0f, 0x7c, 0xf8, 0x07, 0xfe, 0xfc, 0x03, 0xff, 0xff, 0x01, 0xfe, 0xff, 0x00, 0xfc, 0x7f, 0x00,
  0xf8, 0x3f, 0x00, 0xf0, 0x1f, 0x00, 0xe0, 0x0f, 0x00, 0xc0, 0x07, 0x00, 0x80, 0x03, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'x-3x', 24x24px
const unsigned char epd_bitmap_x_3x[] = {
  0x10, 0x00, 0x08, 0x38, 0x00, 0x1c, 0x7c, 0x00, 0x3e, 0xfe, 0x00, 0x7f, 0xff, 0x81, 0x7f, 0xfe,
  0xc3, 0x3f, 0xfc, 0xe7, 0x1f, 0xf8, 0xff, 0x0f, 0xf0, 0xff, 0x07, 0xe0, 0xff, 0x03, 0xc0, 0xff,
  0x01, 0x80, 0xff, 0x00, 0x80, 0xff, 0x01, 0xc0, 0xff, 0x03, 0xe0, 0xff, 0x07, 0xf0, 0xff, 0x0f,
  0xf8, 0xf7, 0x1f, 0xfc, 0xe3, 0x3f, 0xfe, 0xc1, 0x7f, 0xff, 0x80, 0x7f, 0x7e, 0x00, 0x3f, 0x3c,
  0x00, 0x1e, 0x18, 0x00, 0x0c, 0x00, 0x00, 0x00
};

// 'thumb-up-3x', 24x24px
const unsigned char epd_bitmap_thumb_up_3x[] = {
  0x00, 0x70, 0x00, 0x00, 0x70, 0x00, 0x00, 0x78, 0x00, 0x00, 0x38, 0x00, 0x00, 0x3c, 0x00, 0x00,
  0x3c, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x3f, 0x00, 0xc7, 0xff, 0x1f, 0xc7, 0xff,
  0x1f, 0xc7, 0xff, 0x1f, 0xc7, 0xff, 0x0f, 0xc7, 0xff, 0x0f, 0xc7, 0xff, 0x0f, 0xc7, 0xff, 0x07,
  0xc7, 0xff, 0x07, 0xc7, 0xff, 0x07, 0xc7, 0xff, 0x03, 0xc7, 0xff, 0x03, 0xc7, 0xff, 0x03, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


// 'card-id', 24x24px
const unsigned char epd_card_id_3x[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x24, 0x00, 0x00, 0xc7, 0x00, 0x00,
  0x81, 0x00, 0xfe, 0x83, 0x7f, 0x02, 0xff, 0xc0, 0x03, 0x00, 0x80, 0xf3, 0x01, 0x80, 0x1b, 0xf3,
  0x8f, 0x0b, 0x03, 0x80, 0x0b, 0x03, 0x80, 0x0b, 0x03, 0x80, 0x1b, 0xf3, 0x9f, 0xfb, 0x03, 0x80,
  0x03, 0x00, 0x80, 0x03, 0x00, 0x80, 0xfb, 0xff, 0x9f, 0x03, 0x00, 0x80, 0x02, 0x00, 0x80, 0xfe,
  0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'warning-3x', 24x24px
const unsigned char epd_bitmap_warning_3x[] = {
  0x00, 0x0e, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x1f, 0x00, 0x80, 0x1f, 0x00, 0x80, 0x3f, 0x00, 0x80,
  0x3f, 0x00, 0xc0, 0x7f, 0x00, 0xc0, 0x7f, 0x00, 0xe0, 0xff, 0x00, 0xe0, 0xf1, 0x00, 0xf0, 0xf1,
  0x00, 0xf0, 0xf1, 0x01, 0xf0, 0xf1, 0x01, 0xf8, 0xf1, 0x03, 0xf8, 0xf1, 0x03, 0xfc, 0xff, 0x07,
  0xfc, 0xff, 0x07, 0xfe, 0xff, 0x07, 0xfe, 0xf1, 0x0f, 0xfe, 0xf1, 0x0f, 0xff, 0xf1, 0x1f, 0xff,
  0xff, 0x1f, 0xff, 0xff, 0x1f, 0xff, 0xff, 0x1f
};

// 'timer-3x', 24x24px
const unsigned char epd_bitmap_timer_3x[] = {
  0xc0, 0x7f, 0x00, 0xc0, 0x7f, 0x00, 0xc0, 0x7f, 0x00, 0x80, 0x3f, 0x00, 0xe0, 0x3f, 0x00, 0xf0,
  0x1f, 0x04, 0xf8, 0x00, 0x02, 0x3c, 0x00, 0x03, 0x1e, 0xc0, 0x01, 0x0e, 0xe0, 0x00, 0x0f, 0x70,
  0x1c, 0x07, 0x38, 0x1c, 0x07, 0x1e, 0x1c, 0x07, 0x0e, 0x1c, 0x07, 0x0e, 0x1c, 0x07, 0x00, 0x1c,
  0x0f, 0x00, 0x1e, 0x0e, 0x00, 0x0e, 0x1e, 0x00, 0x0f, 0x3c, 0x80, 0x07, 0xf8, 0xe0, 0x03, 0xf0,
  0xff, 0x01, 0xe0, 0xff, 0x00, 0x80, 0x3f, 0x00
};

// 'signal', 8x8px
const unsigned char epd_bitmap_signal[] = {
  0x40, 0x50, 0x50, 0x54, 0x54, 0x55, 0x55, 0x55
};

// 'signal-2x', 16x16px
const unsigned char epd_bitmap_signal_2x[] = {
  0x00, 0x30, 0x00, 0x30, 0x00, 0x33, 0x00, 0x33, 0x00, 0x33, 0x00, 0x33, 0x30, 0x33, 0x30, 0x33,
  0x30, 0x33, 0x30, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33
};

// 'link-intact-2x', 16x16px
const unsigned char epd_bitmap_link_intact_2x[] = {
  0x00, 0x3c, 0x00, 0x7e, 0x00, 0xe6, 0x00, 0xc0, 0x00, 0xc0, 0xe0, 0xe3, 0xf0, 0x73, 0x38, 0x38,
  0x1c, 0x1c, 0xce, 0x0f, 0xc7, 0x07, 0x03, 0x00, 0x03, 0x00, 0x67, 0x00, 0x7e, 0x00, 0x3c, 0x00
};

// 'loop-circular-2x', 16x16px
const unsigned char epd_bitmap_loop_circular_2x[] = {
  0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0xf0, 0x00, 0x38, 0x00, 0x18, 0x30, 0x0c, 0x78, 0x0c, 0xfc,
  0x3f, 0x30, 0x1e, 0x30, 0x0c, 0x18, 0x00, 0x1c, 0x00, 0x0f, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00
};

// 'loop-circular-2x_120', 16x16px
const unsigned char epd_bitmap_loop_circular_2x_120[] = {
  0x00, 0x00, 0x00, 0x0f, 0x00, 0x07, 0x00, 0x0f, 0x00, 0x0f, 0x08, 0x31, 0x0c, 0x18, 0x08, 0x30,
  0x0c, 0x10, 0x18, 0x30, 0x8c, 0x10, 0xf0, 0x00, 0xf0, 0x00, 0xe0, 0x00, 0xf0, 0x00, 0x00, 0x00
};
// 'loop-circular-2x_60', 16x16px
const unsigned char epd_bitmap_loop_circular_2x_60[] = {
  0x00, 0x00, 0x08, 0x00, 0x50, 0x05, 0xf8, 0x07, 0x38, 0x1a, 0x78, 0x08, 0x40, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x10, 0x1e, 0x58, 0x1c, 0xe0, 0x1f, 0xa0, 0x0a, 0x00, 0x10, 0x00, 0x00
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 48)
const int epd_bitmap_allArray_LEN = 1;
const unsigned char* epd_bitmap_allArray[6] = {
  epd_bitmap_check_3x,
  epd_bitmap_x_3x,
  epd_bitmap_thumb_up_3x,
  epd_card_id_3x,
  epd_bitmap_warning_3x,
  epd_bitmap_timer_3x
};

void writeStringToEEPROM(int startAddr, String data) {
  EEPROM.begin(eepromSize);
  for (int i = 0; i < data.length(); i++) {
    EEPROM.write(startAddr + i, data[i]);
  }
  EEPROM.write(startAddr + data.length(), '\0');
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
  return data.substring(0, data.length() - 1);
}

void bukaAP(String _text) {
  modeAP = true;
  Serial.println("");
  Serial.println(_text);
  WiFi.disconnect();
  delay(1000);

  String ssidString = "SiAPP-Config32-" + chipID;
  const char* ssid = ssidString.c_str();
  const char* password = "siap$bos";

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  server.begin();
  IPAddress IP = WiFi.softAPIP();
  Serial.println("");
  Serial.print("AP: ");
  Serial.println(ssid);
  Serial.print("IP Address (AP): ");
  Serial.println(IP);

  u8g2.clearBuffer();
  drawWrappedText("MODE AKSES POIN", screenWidth / 2, 10, screenWidth, u8g2_font_luBIS08_tf);
  drawWrappedText(ssid, screenWidth / 2, 24, screenWidth, u8g2_font_7x13_tf);
  drawWrappedText(password ? password : "<Tidak Ada Password>", screenWidth / 2, 45, screenWidth, u8g2_font_7x13_tf);
  drawWrappedText(IP.toString().c_str(), screenWidth / 2, 60, screenWidth, u8g2_font_7x13_tf);
  u8g2.sendBuffer();
}

void handleReboot(AsyncWebServerRequest* request) {
  request->send_P(200, "text/html", selesai_html);
  delay(2000);
  ESP.restart();
}

void handleLogin(AsyncWebServerRequest* request) {
  boolean usernameOK = false;
  boolean passwordOK = false;

  usernameLogin = request->arg("username");
  passwordLogin = request->arg("password");

  if (usernameLogin == "siapconfig") {
    usernameOK = true;
  }

  if (passwordLogin == "siap$bos") {
    passwordOK = true;
  }

  if (usernameOK && passwordOK) {
    String formattedHtml = String(index_html);
    formattedHtml.replace("%IDCHIP%", chipID);
    formattedHtml.replace("%c", "/");
    formattedHtml.replace("%SSID_NEW%", ssidNew);
    formattedHtml.replace("%PASS_NEW%", passNew);
    formattedHtml.replace("%NODEVICE%", nodevice);
    formattedHtml.replace("%HOST%", hostNew);

    // Send the response
    request->send_P(200, "text/html", formattedHtml.c_str());
  } else if (!usernameOK && passwordOK) {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "Login Gagal - Username Login Salah!");
    formattedHtml.replace("%c", "/");
    request->send_P(404, "text/html", formattedHtml.c_str());
  } else if (usernameOK && !passwordOK) {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "Login Gagal - Password Login Salah!");
    formattedHtml.replace("%c", "/");
    request->send_P(404, "text/html", formattedHtml.c_str());
  } else if (usernameLogin == "" || passwordLogin == "") {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "Login Gagal - Login dulu ya..");
    formattedHtml.replace("%c", "/");
    request->send_P(404, "text/html", formattedHtml.c_str());
  } else {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "Login Gagal - Username dan Password Login Salah!");
    formattedHtml.replace("%c", "/");
    request->send_P(404, "text/html", formattedHtml.c_str());
  }
}

void handleForm(AsyncWebServerRequest* request) {
  ssidNew = request->arg("ssidNew");
  passNew = request->arg("passNew");
  nodeviceNew = request->arg("nodevice");
  hostNew = request->arg("host");

  Serial.println("");
  Serial.println("SSID: ");
  Serial.println(ssidNew);
  Serial.println("PASS: ");
  Serial.println(passNew);
  Serial.println("nodevice: ");
  Serial.println(nodeviceNew);
  Serial.println("HOST: ");
  Serial.println(hostNew);
  Serial.println("");

  writeStringToEEPROM(0, ssidNew);
  writeStringToEEPROM(64, passNew);
  writeStringToEEPROM(128, nodeviceNew);
  writeStringToEEPROM(192, hostNew);


  // jangan menyimpan config kosong
  if (ssidNew == "" && hostNew == "" && nodeviceNew == "") {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "GAGAL melakukan Konfigurasi");
    formattedHtml.replace("%c", "/setting");
    request->send_P(404, "text/html", formattedHtml.c_str());
  } else {
    // Save values to EEPROM
    writeStringToEEPROM(0, ssidNew);
    writeStringToEEPROM(64, passNew);
    writeStringToEEPROM(128, nodeviceNew);
    writeStringToEEPROM(192, hostNew);


    String formattedHtml = String(sukses_html);
    formattedHtml.replace("%IDCHIP%", chipID);
    formattedHtml.replace("%SSID_NEW%", ssidNew);
    formattedHtml.replace("%PASS_NEW%", passNew);
    formattedHtml.replace("%NODEVICE%", nodeviceNew);
    formattedHtml.replace("%HOST%", hostNew);
    request->send_P(200, "text/html", formattedHtml.c_str());

    delay(2000);

    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssidNew.c_str(), passNew.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
      delay(1000);
      Serial.print(".");
      attempts++;
    }

    if (WiFi.status() != WL_CONNECTED) {
      displayIconStatusText(ssidNew.c_str(), "Gagal konek WiFi! Mulai mode AP..", epd_bitmap_x_3x);
      delay(2000);
      // Jika tidak terkoneksi dalam 30 detik, pindah ke mode Akses Poin
      bukaAP("Tidak dapat terhubung ke WiFi. Memulai mode Akses Poin...");
    } else {
      Serial.println("");
      Serial.println("Terhubung Ke Jaringan");
    }
  }
}

void checkButton() {
  int buttonPressed = digitalRead(buttonPin);

  if (tombolDitekan == false) {
    buttonPressTime = millis();
  }

  if (buttonPressed == LOW) {
    tombolDitekan = true;
    digitalWrite(0, HIGH);
    delay(100);
    digitalWrite(0, LOW);
    delay(100);

    Serial.print("#");
    if (millis() - buttonPressTime > 5000) {
      tombolDitekan = false;

      displayIconStatusText(ssidNew.c_str(), "Tombol SET aktif! Mulai mode AP..", epd_bitmap_check_3x);
      delay(2000);
      bukaAP("Tombol telah ditekan selama 5 detik. Memulai mode Akses Poin...");
    }
  } else {
    tombolDitekan = false;
  }
}

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  u8g2.begin();

  delay(1000);

  boot("Booting...");

  chipID = idChip();

  ssidNew = readStringFromEEPROM(0);
  passNew = readStringFromEEPROM(64);
  nodeviceNew = readStringFromEEPROM(128);
  hostNew = readStringFromEEPROM(192);

  if (ssidNew != "" && passNew != "") {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssidNew.c_str(), passNew.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
      delay(1000);
      Serial.print(".");
      bootLoad("Menyambung ke WiFi");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      displayIconStatusText(ssidNew.c_str(), "Tersambung ke WiFi", epd_bitmap_check_3x);

      modeAP = false;
      Serial.println("");
      Serial.println("Terhubung Ke Jaringan");

      ssidNew = readStringFromEEPROM(0);
      passNew = readStringFromEEPROM(64);
      nodeviceNew = readStringFromEEPROM(128);
      hostNew = readStringFromEEPROM(192);

      strcpy(nodevice, nodeviceNew.length() > 0 ? nodeviceNew.c_str() : nodevice);

      Serial.print("SSID: ");
      Serial.println(ssidNew);
      Serial.print("pass: ");
      Serial.println(passNew);
      Serial.print("nodevice: ");
      Serial.println(nodeviceNew);
      Serial.print("host: ");
      Serial.println(hostNew);
      Serial.println();
    } else {
      displayIconStatusText(ssidNew.c_str(), "Gagal Konek WiFi! Mulai mode AP..", epd_bitmap_x_3x);
      delay(2000);
      bukaAP("Tidak dapat terhubung ke WiFi. Memulai mode Akses Poin...");
    }
  } else {
    displayIconStatusText(ssidNew.c_str(), "SSID Kosong! Mulai mode AP..", epd_bitmap_x_3x);
    delay(2000);
    bukaAP("SSID dan password tidak ditemukan di EEPROM. Memulai mode Akses Poin...");
  }

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(0, OUTPUT);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    String formattedHtml = String(login_html);
    formattedHtml.replace("%IDCHIP%", chipID);
    formattedHtml.replace("%c", "/");
    request->send_P(200, "text/html", formattedHtml.c_str());
  });

  server.on("/action_page", HTTP_POST, handleForm);
  server.on("/reboot", HTTP_GET, handleReboot);
  server.on("/setting", HTTP_POST, handleLogin);

  server.onNotFound([](AsyncWebServerRequest* request) {
    String formattedHtml = String(error_html);
    formattedHtml.replace("%s", "404 - Halaman Tidak ditemukan");
    formattedHtml.replace("%c", "/");
    request->send_P(404, "text/html", error_html);
  });

  delay(2000);

  server.begin();

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  Serial.print("Chip ID: ");
  Serial.println(chipID);

  timeClient.begin();
}

void loop() {
  if (modeAP == false) {
    checkButton();

    if (WiFi.status() == WL_CONNECTED) {
      homeLCD();

      // Baca tag RFID
      if (bacaTag()) {
        Serial.print("ID Tag: ");
        Serial.println(IDTAG);
      }

      lastTimeConnected = millis();
    } else {
      if (millis() - lastTimeConnected >= 30000) {
        displayIconStatusText(ssidNew.c_str(), "Gagal Konek WiFi! Mulai mode AP..", epd_bitmap_x_3x);
        delay(2000);

        bukaAP("30 detik tidak dapat menyambung WiFi, buka mode AP");
      }
    }
  }
}

String idChip() {
  String id, buf_id;
  uint64_t chipid;
  char ssid[13];
  chipid = ESP.getEfuseMac();  //The chip ID is essentially its MAC address(length: 6 bytes).
  uint16_t chip = (uint16_t)(chipid >> 32);
  snprintf(ssid, 13, "%04X%08X", chip, (uint32_t)chipid);
  for (int i = 0; i < 12; i++) {
    buf_id += String(ssid[i]);
  }
  id = buf_id;
  buf_id = "";
  // Serial.println(id);

  return id;
}

int bacaTag() {
  if (!tungguRespon && modeAP == false) {
    u8g2.drawXBM(112, 0, 16, 16, epd_bitmap_loop_circular_2x_60);
    u8g2.sendBuffer();
  }

  if (!mfrc522.PICC_IsNewCardPresent())
    return 0;

  if (!mfrc522.PICC_ReadCardSerial())
    return 0;

  memset(IDTAG, 0, sizeof(IDTAG));
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    snprintf(IDTAG, sizeof(IDTAG), "%s%02X", IDTAG, mfrc522.uid.uidByte[i]);
  }

  return 1;
}

void drawLoadingBar(int centerX, int centerY, int width, int height, float progress) {
  u8g2.drawFrame(centerX - width / 2, centerY - height / 2, width, height);
  int barWidth = width * progress;
  u8g2.drawBox(centerX - width / 2, centerY - height / 2, barWidth, height);
}

void drawWrappedText(const char* text, int centerX, int centerY, int maxWidth, const uint8_t* font) {
  // u8g2.setFont(u8g2_font_7x13_tf);
  // u8g2.setFont(u8g2_font_luBS08_tf);

  u8g2.setFont(font);

  const char* delimiter = " ";
  char* mutableText = strdup(text);

  char wrappedText[128] = "";  // Assuming the buffer size is sufficient

  char* token = strtok(mutableText, delimiter);

  while (token != NULL) {
    char temp[128];
    strcpy(temp, wrappedText);
    strcat(temp, token);

    // Check if adding the next word exceeds the maxWidth
    if (u8g2.getStrWidth(temp) > maxWidth) {
      // Draw the wrapped text and move to the next line
      u8g2.drawStr((centerX - u8g2.getStrWidth(wrappedText) / 2 + 5), centerY, wrappedText);
      centerY += u8g2.getFontAscent() - u8g2.getFontDescent();
      strcpy(wrappedText, "");
    }

    strcat(wrappedText, token);
    strcat(wrappedText, " ");
    token = strtok(NULL, delimiter);
  }

  // Draw the last line of wrapped text
  u8g2.drawStr((centerX - u8g2.getStrWidth(wrappedText) / 2) + 5, centerY, wrappedText);

  free(mutableText);
}

void boot(const char* textboot) {
  // Set the position and size of the loading bar
  int barWidth = 100;
  int barHeight = 10;
  int barCenterX = screenWidth / 2;
  int barCenterY = screenHeight / 2;

  // Draw the loading bar with a changing progress value
  for (float progress = 0.0; progress <= 1.0; progress += 0.1) {
    u8g2.clearBuffer();
    drawLoadingBar(barCenterX, barCenterY, barWidth, barHeight, progress);
    drawWrappedText(textboot, screenWidth / 2, 50, screenWidth, u8g2_font_7x13_tf);
    u8g2.sendBuffer();
    delay(1);  // Adjust the delay based on your desired animation speed
  }
}


void bootLoad(const char* textboot) {
  // Set the position and size of the loading bar
  int barWidth = 100;
  int barHeight = 10;
  int barCenterX = screenWidth / 2;
  int barCenterY = screenHeight / 2;
  float pis = 0.4;

  // Draw the loading bar with a changing progress value
  for (int i = 1; i <= 2; i++) {
    float pos = pis * i;
    u8g2.clearBuffer();

    // Calculate elapsed seconds
    unsigned long elapsedSeconds = (millis() - startTimeBootLoad) / 1000;

    // Calculate minutes and seconds
    int minutes = elapsedSeconds / 60;
    int seconds = elapsedSeconds % 60;

    // Convert minutes and seconds to a string
    char timeStr[10];
    sprintf(timeStr, "%02d:%02d", minutes, seconds);

    drawLoadingBar(barCenterX, barCenterY, barWidth, barHeight, pos);
    drawWrappedText(timeStr, screenWidth / 2, 10, screenWidth, u8g2_font_7x13_tf);
    drawWrappedText(textboot, screenWidth / 2, 50, screenWidth, u8g2_font_7x13_tf);
    u8g2.sendBuffer();
    delay(5);  // Sesuaikan delay berdasarkan kecepatan animasi yang diinginkan
  }
}

void iconBMP(int pilih_iconBMP) {
  if (pilih_iconBMP) {
    u8g2.drawXBM(0, 28, 24, 24, epd_bitmap_allArray[(pilih_iconBMP - 1)]);
  }
}


void iconCenter(const char* _kode) {
  u8g2.clearBuffer();
  if (nom <= 11) {
    // Silang
    String errorMessage = "Error! ";
    errorMessage += _kode;
    drawWrappedText(errorMessage.c_str(), screenWidth / 2, 10, screenWidth, u8g2_font_7x13_tf);

    drawWrappedText("Segera Hubungi Admin", screenWidth / 2, 50, screenWidth, u8g2_font_7x13_tf);
    u8g2.drawXBM((screenWidth / 2) - 12, (screenHeight / 2) - 12, 24, 24, epd_bitmap_allArray[1]);
  } else if (nom <= 16) {
    // Warning
    drawWrappedText("Akses Ditolak", screenWidth / 2, 10, screenWidth, u8g2_font_7x13_tf);
    u8g2.drawXBM((screenWidth / 2) - 12, (screenHeight / 2) - 12, 24, 24, epd_bitmap_allArray[4]);
  } else if (nom <= 30) {
    // check list
    drawWrappedText("Success", screenWidth / 2, 10, screenWidth, u8g2_font_7x13_tf);
    u8g2.drawXBM((screenWidth / 2) - 12, (screenHeight / 2) - 12, 24, 24, epd_bitmap_allArray[0]);
  }
  u8g2.sendBuffer();
  nom = 0;
}

void noLoadBarJustText(const char* _textnya) {
  u8g2.clearBuffer();
  drawLoadingBar(screenWidth / 2, screenHeight / 2, 100, 10, 0.7);
  drawWrappedText(_textnya, screenWidth / 2, 50, screenWidth, u8g2_font_7x13_tf);
  u8g2.sendBuffer();
}

void displayIconStatusText(const char* _title, const char* _pesan, const uint8_t* _icon) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBIS08_tf);
  drawWrappedText(_title, (screenWidth / 2) - 5, 10, screenWidth, u8g2_font_luBIS08_tf);
  u8g2.drawXBM(52, 16, 24, 24, _icon);
  drawWrappedText(_pesan, screenWidth / 2, 50, screenWidth, u8g2_font_7x13_tf);
  u8g2.sendBuffer();
}

void homeLCD() {
  u8g2.clearBuffer();

  int rssi = WiFi.RSSI();
  int barsignal;

  char rssiString[10];
  sprintf(rssiString, "%d", rssi);

  if (rssi >= (-70)) {
    barsignal = 16;
  } else if (rssi >= (-80)) {
    barsignal = 12;
  } else if (rssi >= (-90)) {
    barsignal = 8;
  } else if (rssi >= (-100)) {
    barsignal = 4;
  } else {
    barsignal = 0;
  }

  u8g2.drawXBM(0, 0, barsignal, 16, epd_bitmap_signal_2x);
  u8g2.setFont(u8g2_font_luBIS10_tf);
  u8g2.drawStr(40, 12, "SiAPP");
  // u8g2.setFont(u8g2_font_6x10_tf);
  // u8g2.drawStr(96, 12, rssiString);
  // u8g2.drawStr(116, 12, "db");

  // Text to display in the center
  const char* centerText;
  if (tungguRespon) {
    // x, y, length.x, length.y
    u8g2.drawXBM(112, 0, 16, 16, epd_bitmap_link_intact_2x);
    iconBMP(6);
    centerText = "Menunggu Respon Server...";
  } else {
    u8g2.drawXBM(112, 0, 16, 16, epd_bitmap_loop_circular_2x);
    iconBMP(4);
    centerText = "Tempelkan Kartu!";
    ntp();
  }

  if (rssi >= (-60)) {
    u8g2.drawStr(15, 6, "+");
  }

  drawWrappedText(centerText, 72, 40, screenWidth * 0.75, u8g2_font_luBS08_tf);
  u8g2.sendBuffer();
}

void ntp() {
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  //Get a time structure
  struct tm* ptm = gmtime((time_t*)&epochTime);
  String weekDay = weekDays[timeClient.getDay()];
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon + 1;

  // String currentMonth_2digit;

  // if (currentMonth < 10) {
  //   currentMonth_2digit = "0" + String(currentMonth);
  // } else {
  //   currentMonth_2digit = String(currentMonth);
  // }

  String currentMonthName = months[currentMonth - 1];
  int currentYear = ptm->tm_year + 1900;
  int lastTwoDigits = currentYear % 100;

  String hariTanggal = weekDay + ", " + monthDay + " " + currentMonthName + " " + lastTwoDigits;
  // String hariTanggal = weekDay + ", " + monthDay + " " + currentMonthName + " " + currentYear;
  // Serial.print("Current date: ");
  // Serial.println(hariTanggal);

  String timestamp = timeClient.getFormattedTime();
  // String timestamp = String(currentYear) + "-" + currentMonth_2digit + "-" + String(monthDay) + " " + timeClient.getFormattedTime();
  // Serial.print("timestamp: ");
  // Serial.println(timestamp);
  // Serial.println("");

  drawWrappedText(timestamp.c_str(), 102, 24, screenWidth, u8g2_font_6x10_tf);
  drawWrappedText(hariTanggal.c_str(), screenWidth / 2, 64, screenWidth, u8g2_font_6x10_tf);
}