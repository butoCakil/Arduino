#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define START_BTN 0
#define STOP_BTN 2

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);
// SW_I2C untuk SLOW Frame Rate
// HW_I2C untuk HIGH Frame Rate

// 'emo_smile', 48x48px
const unsigned char epd_bitmap_emo_smile[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x7f, 0x00, 0x00, 0x00, 0xc0,
  0x0f, 0xf0, 0x03, 0x00, 0x00, 0xe0, 0x01, 0x80, 0x07, 0x00, 0x00, 0x78, 0x00, 0x00, 0x1e, 0x00,
  0x00, 0x1c, 0x00, 0x00, 0x38, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x70, 0x00, 0x00, 0x07, 0x00, 0x00,
  0xe0, 0x00, 0x80, 0x03, 0x00, 0x00, 0xc0, 0x01, 0x80, 0x01, 0x00, 0x00, 0x80, 0x01, 0xc0, 0x00,
  0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x07, 0x60, 0x00, 0x00, 0x00, 0x00, 0x06,
  0x60, 0x00, 0x02, 0x40, 0x00, 0x06, 0x30, 0x00, 0x07, 0xe0, 0x00, 0x0c, 0x30, 0x80, 0x0f, 0xf0,
  0x01, 0x0c, 0x30, 0x80, 0x0f, 0xf0, 0x01, 0x0c, 0x18, 0x80, 0x0f, 0xf0, 0x01, 0x18, 0x18, 0x80,
  0x0f, 0xf0, 0x01, 0x18, 0x18, 0x80, 0x0f, 0xf0, 0x01, 0x18, 0x18, 0x80, 0x0f, 0xf0, 0x01, 0x18,
  0x18, 0x00, 0x07, 0xe0, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00,
  0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x30, 0x0c,
  0x00, 0x00, 0x30, 0x0c, 0x30, 0x1c, 0x00, 0x00, 0x38, 0x0c, 0x60, 0x38, 0x00, 0x00, 0x1c, 0x06,
  0x60, 0x70, 0x00, 0x00, 0x0e, 0x06, 0xe0, 0xe0, 0x00, 0x00, 0x07, 0x07, 0xc0, 0xc0, 0x01, 0x80,
  0x03, 0x03, 0x80, 0x81, 0x0f, 0xf0, 0x81, 0x01, 0x80, 0x03, 0xff, 0xff, 0xc0, 0x01, 0x00, 0x07,
  0xf8, 0x1f, 0xe0, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x70, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x38, 0x00,
  0x00, 0x78, 0x00, 0x00, 0x1e, 0x00, 0x00, 0xe0, 0x01, 0x80, 0x07, 0x00, 0x00, 0xc0, 0x0f, 0xf0,
  0x03, 0x00, 0x00, 0x00, 0xfe, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'emo-smile-brush', 48x48px
const unsigned char epd_bitmap_emo_smile_brush[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x40, 0x00, 0x00, 0x00, 0xc0,
  0x01, 0x00, 0x30, 0x00, 0x00, 0x80, 0xff, 0xbf, 0x11, 0x00, 0x00, 0x00, 0x1f, 0xf8, 0x03, 0x00,
  0x00, 0xe0, 0x11, 0x90, 0x13, 0x00, 0x00, 0x60, 0x00, 0x00, 0x9f, 0x00, 0x00, 0x39, 0x00, 0x00,
  0xdc, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x18, 0x00, 0x00, 0x0e, 0x00, 0x00, 0xf0, 0x03, 0x00, 0x8f,
  0x01, 0x80, 0xe1, 0x0b, 0x00, 0xc3, 0x03, 0xc0, 0xc3, 0x07, 0x00, 0xe1, 0x07, 0xe0, 0xc7, 0x07,
  0x80, 0xe1, 0x07, 0xe0, 0x87, 0x07, 0xe0, 0xe1, 0x07, 0xe0, 0x87, 0x01, 0xf0, 0xe0, 0x07, 0xe0,
  0x87, 0x01, 0xe0, 0xe0, 0x07, 0xe0, 0x07, 0x01, 0xc0, 0xe0, 0x07, 0xe0, 0x07, 0x03, 0x40, 0xe0,
  0x07, 0xe0, 0x07, 0x07, 0x40, 0xe0, 0x07, 0xe0, 0x07, 0x27, 0x40, 0xe0, 0x03, 0xc0, 0x03, 0x07,
  0x40, 0x80, 0x01, 0x80, 0x01, 0x03, 0x40, 0x00, 0x08, 0x00, 0x00, 0x03, 0x40, 0x03, 0x08, 0x20,
  0x40, 0x07, 0x80, 0x02, 0x00, 0x20, 0x61, 0x1f, 0xc0, 0x0e, 0x00, 0x00, 0x70, 0x13, 0xc0, 0x1e,
  0x00, 0x00, 0xbc, 0x01, 0xd0, 0x7d, 0x20, 0x00, 0xbe, 0x08, 0x80, 0xfd, 0x27, 0xc0, 0xdd, 0x00,
  0x88, 0x3b, 0x3f, 0x7c, 0x5e, 0x20, 0x80, 0xfb, 0xf0, 0xc7, 0x6f, 0x00, 0xc0, 0xf7, 0x83, 0xe0,
  0x27, 0x00, 0x60, 0xec, 0xff, 0xff, 0x33, 0x00, 0x20, 0xdc, 0xff, 0xff, 0x1d, 0x00, 0x00, 0x3e,
  0xff, 0xff, 0x0e, 0x00, 0x00, 0xd3, 0xfe, 0x3f, 0x0f, 0x00, 0x00, 0x81, 0xff, 0xef, 0x0f, 0x00,
  0x00, 0x00, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0xfe, 0x23, 0x18, 0x00, 0x00, 0x00, 0xc7, 0x01,
  0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x10, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// '8iEb4KGBT (2)', 48x48px
const unsigned char epd_bitmap_spiderman[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xf8, 0x1f, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0xc0, 0x9f, 0xf9, 0x03, 0x00,
  0x00, 0xe0, 0x03, 0xc1, 0x07, 0x00, 0x00, 0xf8, 0x86, 0x43, 0x1f, 0x00, 0x00, 0x3c, 0xfc, 0x7f,
  0x3c, 0x00, 0x00, 0x7c, 0x0e, 0x71, 0x3c, 0x00, 0x00, 0x7e, 0x0b, 0xa1, 0x7d, 0x00, 0x00, 0xf7,
  0x08, 0x11, 0xef, 0x00, 0x80, 0x7b, 0xf8, 0x1f, 0xde, 0x01, 0x80, 0xfb, 0xb8, 0x39, 0xdf, 0x01,
  0xc0, 0xfd, 0x17, 0xe9, 0xbf, 0x03, 0xc0, 0xff, 0xa3, 0xcd, 0xff, 0x03, 0xc0, 0x9c, 0xf7, 0xcf,
  0x71, 0x03, 0xe0, 0x0e, 0x7e, 0xfd, 0x70, 0x07, 0xe0, 0x0e, 0x4c, 0x77, 0x70, 0x07, 0x60, 0x0e,
  0xd8, 0x3b, 0x60, 0x07, 0xe0, 0x0e, 0xf8, 0x1f, 0xe0, 0x07, 0xe0, 0x07, 0x70, 0x0c, 0xe0, 0x07,
  0x60, 0x06, 0x60, 0x0e, 0x60, 0x06, 0x60, 0x0e, 0x60, 0x06, 0x60, 0x06, 0xe0, 0x0e, 0xe0, 0x07,
  0x60, 0x07, 0xe0, 0x0e, 0xe0, 0x07, 0x70, 0x07, 0xe0, 0x1f, 0xf0, 0x0f, 0xf0, 0x07, 0xc0, 0x1c,
  0xf0, 0x0f, 0x38, 0x03, 0xc0, 0x39, 0x78, 0x1e, 0x9c, 0x03, 0xc0, 0xf1, 0x7f, 0xfe, 0x9f, 0x03,
  0x80, 0xf3, 0xff, 0xff, 0xcf, 0x01, 0x80, 0xdf, 0x67, 0xe6, 0xf3, 0x01, 0x00, 0x0f, 0x23, 0xc4,
  0xe1, 0x00, 0x00, 0x0e, 0x27, 0xe4, 0x70, 0x00, 0x00, 0x1c, 0x7d, 0xbe, 0x38, 0x00, 0x00, 0xbc,
  0xf0, 0x0f, 0x3d, 0x00, 0x00, 0xf8, 0x20, 0x04, 0x1f, 0x00, 0x00, 0xe0, 0x13, 0xcc, 0x07, 0x00,
  0x00, 0xc0, 0x3f, 0xfc, 0x03, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x1f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 304)
const int epd_bitmap_allArray_LEN = 1;

const unsigned char* epd_bitmap_allArray[3] = {
  epd_bitmap_emo_smile,
  epd_bitmap_emo_smile_brush,
  epd_bitmap_spiderman
};

int modeTampilan;
int maksMode = 2;

void setup() {
  pinMode(START_BTN, INPUT_PULLUP);
  pinMode(STOP_BTN, INPUT_PULLUP);

  SPI.begin();
  u8g2.begin();

  tampilanText("", "Mulai Test ESP32");
}

void loop() {
  if (digitalRead(START_BTN) == 0) {
    // Menamabah nilai pada modeTampilan sebanyak 1 (modeTampilan + 1)
    modeTampilan++;

    // TEKAN TAHAN - Menahan agar tombol tidak mengubah mode ketika ditekan terus
    while (digitalRead(START_BTN) == 0) {
    }
    // TEKAN TAHAN - Selesai
  }

  if (digitalRead(STOP_BTN) == 0) {
    // Mengurangi nilai pada modeTampilan sebanyak 1 (modeTampilan - 1)
    modeTampilan--;

    // TEKAN TAHAN - Menahan agar tombol tidak mengubah mode ketika ditekan terus
    while (digitalRead(STOP_BTN) == 0) {
    }
    // TEKAN TAHAN - Selesai
  }

  // Mulai Mengoreksi nilai dari modeTampilan supaya tetap di dalam nilai mode yang tersedia
  // ======================================================================================
  // Aktifkan untuk mengantisipasi jika modeTampilan menjadi lebih dari `2` supaya di jadikan maksimal `2`
  // dan jika modeTampilan menjadi kurang dari NOL supaya di jadikan miniman NOL
  // if (modeTampilan > maksMode) {
  //   modeTampilan = maksMode;
  // }

  // if (modeTampilan < 0) {
  //   modeTampilan = 0;
  // }

  // Aktifkan untuk mengantisipasi jika modeTampilan menjadi lebih dari `2` supaya dikembalikan ke `0`
  // dan jika modeTampilan menjadi kurang dari `0` supaya dikembalikan ke `2`
  if (modeTampilan > maksMode) {
    modeTampilan = 0;
  }

  if (modeTampilan < 0) {
    modeTampilan = maksMode;
  }
  // Selesai Mengoreksi modeTampilan

  // Memanggil Fungsi `void tampilan` dengan mengirim data `modeTampilan` saat ini
  tampilan(modeTampilan);
}

// Fungsi untuk menampilkan Tulisan konten dan Judul
void tampilanText(String konten, String judul) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_luBS08_tf);
  // menampilakan isi dari variabel `konten` yang diterima
  u8g2.drawStr(32, 48, konten.c_str());
  // menampilakan isi dari variabel `judul` yang diterima
  u8g2.drawStr(0, 10, judul.c_str());
  u8g2.sendBuffer();
}
// Fungsi Selesai

// Fingsi Untuk Menampilkan ICON / Gambar berdasarkan urutan Gambar di "epd_bitmap_allArray"
void icon(int _nomorIcon, String _judul) {
  u8g2.clearBuffer();
  u8g2.drawXBM(40, 16, 48, 48, epd_bitmap_allArray[_nomorIcon]);
  u8g2.drawStr(0, 10, _judul.c_str());
  u8g2.sendBuffer();
}
// Fungsi Selesai

// Fungsi untuk memilih tampilan berdasarkan modeTampilan
void tampilan(int _modeTampilan) {
  if (_modeTampilan == 1) {
    // memanggil `void icon` dan mengirim data `1`
    icon(1, "Venom");
  } else if (_modeTampilan == 2) {
    // memanggil `void icon` dan mengirim data `2`
    icon(2, "I'm Spiderman");
  } else {
    // memanggil `void icon` dan mengirim data `0`
    icon(0, "Peter Parker");
  }
}
// Fungsi Selesai