#include "core_esp8266_features.h"
// Membuat array untuk memetakan pesan ke kode bunyi buzzer
const char* buzzerCodes[] = {
  // error
  "400", "_ _",        // 0
  "404", "_..._",      // 1
  "405", "_...._",     // 2
  "406", "_....._",    // 3
  "407", "_....._",    // 4
  "500", "_....._",    // 5
  "501", "_._..._",    // 6
  "502", "_._....._",  // 7
  "505", "_....._._",  // 8
  "515", "_....._",    // 9
  "545", "_....._",    // 10
  "555", "_....._",    // 11
  // akses
  "IDTT", "._..",    // 12
  "HLTM", "_.",      // 13
  "TBPS", "._._..",  // 14
  "TASK", "_._..",   // 15
  "PLAW", "_..",     // 16
  // OK
  "200", "..",    // 17
  "SAPP", "...",  // 18
  "PPBH", "..",   // 19
  "510", "...",   // 20
  "PPPP", "...",  // 21
  "SMPM", "...",  // 22
  "MMMM", "...",  // 23
  "BMPM", "..",   // 24
  "PKBD", "..",   // 25
  "BMPE", "..",   // 26
  "BPSE", "..",   // 27
  "BPEB", "..",   // 28
  "BMIJ", ".."    // 29
};


// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
String weekDays[7] = { "Min", "Sen", "Sel", "Rab", "Kam", "Jum", "Sab" };
String months[12] = { "Jan", "Feb", "Mar", "Apr", "Mei", "Jun", "Jul", "Agt", "Sep", "Okt", "Nov", "Des" };


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

int bacaTag() {
  if (!tungguRespon && modeAPaktif == false) {
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

void buzz_er(String _kode) {
  for (int i = 0; i < _kode.length(); i++) {
    char karakter = _kode.charAt(i);

    if (karakter == '_') {
      // Buzzer berbunyi selama 1 detik
      digitalWrite(BUZ_PIN, HIGH);
      // Bunyi selama 1 detik
      delay(1000);
      // Matikan buzzer
      digitalWrite(BUZ_PIN, LOW);
    } else if (karakter == '.') {
      // Buzzer berbunyi selama 100 mili detik
      digitalWrite(BUZ_PIN, HIGH);
      // Bunyi selama 100 mili detik
      delay(100);
      // Matikan buzzer
      digitalWrite(BUZ_PIN, LOW);
    } else if (karakter == ' ') {
      // Tunda 100 mili detik
      delay(100);
    }

    // Tunda sebelum karakter selanjutnya (jika ada)
    if (i < _kode.length() - 1) {
      delay(100);  // Tunda 100 mili detik sebelum karakter berikutnya
    }
  }

  delay(1000);
}

void buzzBasedOnMessage(const char* message) {
  nom = 0;
  for (int i = 0; i < sizeof(buzzerCodes) / sizeof(buzzerCodes[0]); i += 2) {
    if (strcmp(message, buzzerCodes[i]) == 0) {
      buzz_er(buzzerCodes[i + 1]);
      if (aktifSerialMsg)
        Serial.println(buzzerCodes[i + 1]);
      break;  // Keluar dari loop setelah menemukan kode yang cocok
    }
    nom++;
  }
}

void reconnect() {
  // Loop sampai terhubung ke broker MQTT
  startTimeBootLoad = millis();
  while (!client.connected()) {
    if (aktifSerialMsg)
      Serial.println("Menyambungkan ke MQTT Broker...");

    bootLoad("Menyambungkan ke Server..");
    // Coba terhubung ke broker MQTT
    if (client.connect("NodeMCUClient", mqtt_user, mqtt_password)) {
      if (aktifSerialMsg)
        Serial.println("Tersambung ke MQTT Broker");

      noLoadBarJustText("Tersambung ke Server");

      // Langganan topik yang Anda butuhkan di sini jika diperlukan
      String topic = "responServer_";
      topic += nodevice;
      client.subscribe(topic.c_str(), 0);
    } else {
      buzzBasedOnMessage("400");

      u8g2.clearBuffer();
      drawWrappedText("Gagal terhubung, Koneksi Ulang", screenWidth / 2, screenHeight / 2, screenWidth, u8g2_font_7x13_tf);
      u8g2.sendBuffer();

      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_luBIS08_tf);
      drawWrappedText("SIAPP", screenWidth / 2, 10, screenWidth, u8g2_font_luBIS08_tf);
      u8g2.drawXBM(52, 16, 24, 24, epd_bitmap_x_3x);
      drawWrappedText("Gagal konek Server!", screenWidth / 2, 50, screenWidth, u8g2_font_7x13_tf);
      u8g2.sendBuffer();

      Serial.print("MQTT Gagal, rc=");
      Serial.print(client.state());
      Serial.println("mencoba konek lagi dalam 5 detik");
    }
  }
}

String sendCardIdToServer(String cardId) {
  String jsonResponse = "";
  // Send RFID card data, Chip ID, Node Device, and Key to the server
  String request = "{";
  request += "\"nokartu\":\"" + String(cardId) + "\",";
  request += "\"idchip\":\"" + String(chipID) + "\",";
  request += "\"nodevice\":\"" + String(nodevice) + "\",";
  request += "\"key\":\"" + String(key) + "\",";
  request += "\"ipa\":\"" + WiFi.localIP().toString() + "\"";
  request += "}";

  if (client.connect("NodeMCUClient", mqtt_user, mqtt_password)) {
    String mqttTopic = "dariMCU_" + String(nodevice);

    if (aktifSerialMsg) {
      Serial.println("Tersambung ke MQTT Broker");
      Serial.println("Kirim ke topik: " + mqttTopic + ": " + request);
    }

    client.publish(mqttTopic.c_str(), request.c_str(), 0);

    noLoadBarJustText("Mengirim ke Server");

  } else {
    buzzBasedOnMessage("400");
    Serial.println("Koneksi ke MQTT Broker gagal");

    u8g2.clearBuffer();
    drawWrappedText("Gagal mengirim ke server!", screenWidth / 2, screenHeight / 2, screenWidth, u8g2_font_7x13_tf);
    u8g2.sendBuffer();

    reconnect();
  }

  return jsonResponse;
}

void displayIconStatusText(const char* _title, const char* _pesan, const uint8_t* _icon) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBIS08_tf);
  drawWrappedText(_title, (screenWidth / 2) - 5, 10, screenWidth, u8g2_font_luBIS08_tf);
  u8g2.drawXBM(52, 16, 24, 24, _icon);
  drawWrappedText(_pesan, screenWidth / 2, 50, screenWidth, u8g2_font_7x13_tf);
  u8g2.sendBuffer();
}


void printWifiList() {
  Serial.println("WiFi networks ditemukan:");
  displayIconStatusText(ssidNew.c_str(), "WiFi network ditemukan..", epd_bitmap_check_3x);

  injekHtml = "";
  injekHtml = "<div id=\"listssid\" class=\"listssid\">";

  int ii;
  for (const auto& wifi : wifiList) {
    Serial.print(ii);
    Serial.print(". ");
    Serial.print("SSID: ");
    Serial.print(wifi.ssid);
    Serial.print(", RSSI: ");
    Serial.print(wifi.rssi);
    Serial.print(" dBm, Security: ");
    Serial.println(wifi.security);

    injekHtml += "<a id=\"tmblssid1\" onclick=\"changeSSID('" + wifi.ssid + "')\">" + wifi.ssid + " (" + wifi.rssi + " dBm) " + wifi.security + "</a><br>";

    ii++;
  }

  injekHtml += "</div>";
}

void findWifi() {
  Serial.println("Mencari WiFi...");

  wifiList.clear();

  // Tunggu hingga pemindaian WiFi selesai
  networks = WiFi.scanNetworks();

  // Tambahkan informasi WiFi ke dalam vektor
  for (int i = 0; i < networks; ++i) {
    WifiInfo wifi;
    wifi.ssid = WiFi.SSID(i);
    wifi.rssi = WiFi.RSSI(i);
    switch (WiFi.encryptionType(i)) {
      case ENC_TYPE_NONE:
        wifi.security = "Open";
        break;
      case ENC_TYPE_WEP:
        wifi.security = "WEP";
        break;
      case ENC_TYPE_TKIP:
      case ENC_TYPE_CCMP:
        wifi.security = "WPA/WPA2";
        break;
      default:
        wifi.security = "Unknown";
        break;
    }
    wifiList.push_back(wifi);
  }

  // Tampilkan hasil pemindaian
  if (networks == 0) {
    Serial.println("WiFi tidak ditemukan. REBOOT / Config Manual");
    injekHtml = "WiFi tidak ditemukan. REBOOT / Config Manual";
    displayIconStatusText(ssidNew.c_str(), "WiFi tidak ditemukan. REBOOT / Config Manual", epd_bitmap_x_3x);
    delay(2000);
  } else {
    startTimeBootLoad = millis();
    bootLoad("Memuat hasil Pencarian WiFi...");
    printWifiList();
  }
}

void searchingWifi() {
  startTimeBootLoad = millis();
  bootLoad("Mencari SSID WiFi Sekitar...");
  findWifi();
}


void buzz(int loop) {
  if (loop == 0) {
    digitalWrite(BUZ_PIN, LOW);
  } else if (loop == 1) {
    digitalWrite(BUZ_PIN, HIGH);
    delay(100);
    digitalWrite(BUZ_PIN, LOW);
  } else {
    for (int t = 0; t < loop; t++) {
      digitalWrite(BUZ_PIN, HIGH);
      delay(100);
      digitalWrite(BUZ_PIN, LOW);
      delay(100);
    }
  }
}

void identifyAndProcessJsonResponse(String jsonResponse, char* _nodevice) {
  const char* pesanJSON = "";
  const char* pesanLCD = "";

  // Parse and process JSON response
  // Menghapus karakter newline dan carriage return
  jsonResponse.replace("\n", "");
  jsonResponse.replace("\r", "");
  // Menghapus karakter backslash yang mengganggu
  jsonResponse.replace("\\", "");

  // Menghapus karakter ganda ("") dari awal dan akhir JSON
  jsonResponse = jsonResponse.substring(1, jsonResponse.length() - 1);
  DynamicJsonDocument jsonDoc(1024);
  DeserializationError error = deserializeJson(jsonDoc, jsonResponse);

  if (error) {
    pesanJSON = "500";
    Serial.print("gagal to parse JSON: ");
    Serial.println(error.c_str());

    u8g2.clearBuffer();
    drawWrappedText("Gagal Proses Pesan!", screenWidth / 2, screenHeight / 2, screenWidth, u8g2_font_7x13_tf);
    u8g2.sendBuffer();
  } else {
    // Mengakses elemen-elemen JSON yang benar
    const char* json_id = jsonDoc["respon"][0]["id"];
    const char* json_nodevice = jsonDoc["respon"][0]["nodevice"];
    const char* json_message = jsonDoc["respon"][0]["message"];
    const char* json_info = jsonDoc["respon"][0]["info"];
    const char* json_nokartu = jsonDoc["respon"][0]["nokartu"];

    if (json_nodevice) {
      if (aktifSerialMsg) {
        Serial.print("- id: ");
        Serial.println(json_id);
        Serial.print("- nodevice asal: ");
        Serial.println(_nodevice);
        Serial.print("- nodevice json: ");
        Serial.println(json_nodevice);
        Serial.print("- pesan: ");
        Serial.println(json_message);
        Serial.print("- info: ");
        Serial.println(json_info);
        Serial.print("- nokartu: ");
        Serial.println(json_nokartu);
      }

      if (strcmp(_nodevice, json_nodevice) == 0 && strcmp("406", json_message) != 0) {
        if (aktifSerialMsg) {
          Serial.print("ID & Nomor Device Sesuai! ");
          Serial.println();
        }

        // Find the position of the search string
        const char* startPos = strstr(json_info, "--");

        // If the search string is found, replace it
        while (startPos != nullptr) {
          char buffer[100];  // Adjust the size according to your needs
          strncpy(buffer, json_info, startPos - json_info);
          buffer[startPos - json_info] = '\0';  // Null-terminate the substring
          strcat(buffer, ", ");
          strcat(buffer, startPos + strlen("--"));

          // Copy the modified string back to the json_info
          strcpy(const_cast<char*>(json_info), buffer);

          // Find the position of the search string in the modified text
          startPos = strstr(json_info, "--");
        }

        u8g2.clearBuffer();
        u8g2.drawBox(0, 0, screenWidth, 12);
        u8g2.setDrawColor(0);
        u8g2.setFont(u8g2_font_luBIS08_tf);
        u8g2.drawStr(48, 10, "INFO:");
        u8g2.setDrawColor(1);
        // iconBMP(1);
        drawWrappedText(json_info, (screenWidth / 2), (screenHeight / 2) - 5, screenWidth, u8g2_font_7x13_tf);
        // drawWrappedText(json_info, 75, 25, screenWidth * 0.75, u8g2_font_7x13_tf);
        u8g2.sendBuffer();

        pesanJSON = json_message;
        json_info = "";
      } else {
        Serial.println("ID & Nomor Device Tidak Sesuai...!");
        Serial.println("Permintaan tidak direspon.");

        u8g2.clearBuffer();
        iconBMP(2);
        u8g2.drawBox(0, 0, screenWidth, 12);
        u8g2.setDrawColor(0);
        u8g2.setFont(u8g2_font_luBIS08_tf);
        u8g2.drawStr(32, 10, "WARNING:");
        u8g2.setDrawColor(1);
        drawWrappedText("Device tidak sesuai", 75, screenHeight / 2, screenWidth * 0.75, u8g2_font_7x13_tf);
        // drawWrappedText("Data tidak direspon", screenWidth / 2, screenHeight / 2, screenWidth, u8g2_font_7x13_tf);
        u8g2.sendBuffer();

        pesanJSON = "501";
      }
    } else {
      // Elemen "nodevice" tidak ada dalam JSON
      Serial.println("Elemen \"nodevice\" tidak ada dalam JSON.");
      Serial.println("Permintaan tidak direspon.");

      u8g2.clearBuffer();
      iconBMP(2);
      u8g2.drawBox(0, 0, screenWidth, 12);
      u8g2.setDrawColor(0);
      u8g2.setFont(u8g2_font_luBIS08_tf);
      u8g2.drawStr(36, 10, "SERVER:");
      u8g2.setDrawColor(1);
      drawWrappedText("Data tidak direspon server", 75, screenHeight / 2, screenWidth * 0.75, u8g2_font_7x13_tf);
      // drawWrappedText("Data tidak direspon server", screenWidth / 2, screenHeight / 2, screenWidth, u8g2_font_7x13_tf);
      u8g2.sendBuffer();

      delay(2000);

      u8g2.clearBuffer();
      iconBMP(2);
      u8g2.drawBox(0, 0, screenWidth, 12);
      u8g2.setDrawColor(0);
      u8g2.setFont(u8g2_font_luBIS08_tf);
      u8g2.drawStr(36, 10, "SERVER:");
      u8g2.setDrawColor(1);
      drawWrappedText("Pastikan no device ini Valid", 75, screenHeight / 2, screenWidth * 0.75, u8g2_font_7x13_tf);
      // drawWrappedText("Data tidak direspon server", screenWidth / 2, screenHeight / 2, screenWidth, u8g2_font_7x13_tf);
      u8g2.sendBuffer();

      delay(2000);

      pesanJSON = "502";
    }

    tungguRespon = false;
  }

  // Setelah selesai memproses respon, putuskan koneksi MQTT
  client.disconnect();
  if (aktifSerialMsg)
    Serial.println("Koneksi Selesai");

  // aktikan Buzz sesuai KOde Pesan
  if (pesanJSON) {
    buzzBasedOnMessage(pesanJSON);
    iconCenter(pesanJSON);
    delay(1000);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  if (aktifSerialMsg) {
    Serial.print("Menerima pesan pada topic: ");
    Serial.println(topic);
  }

  for (int i = 0; i < length; i++) {
    receivedMessage += (char)payload[i];
  }

  if (aktifSerialMsg) {
    Serial.print("Pesan: ");
    Serial.println(receivedMessage);
    Serial.println();
  }

  // prosen respon data
  identifyAndProcessJsonResponse(receivedMessage, nodevice);
}
