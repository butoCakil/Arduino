#include <Wire.h>
#include <RFIDshield.h>
#include <LiquidCrystal.h>

#define DETEKSI_TAG   2
#define BUZZER        11

#define JUMLAH_TAG_TERSIMPAN    10
#define PANJANG_NAMA_MAKS       10

// Definisi status operasi SELECT
#define STATUS_PEMBACAAN_OK   0x00 // Status pembacaan tag sukses
#define STATUS_TIDAK_ADA_TAG  0x01 // Status jika tidak ada tag yg terdeteksi
// Status jika terjadi tumbukan tag, dimana tag yang terdeteksi lebih dari satu
#define STATUS_TUMBUKAN_TAG   0x0A

// Definisi jenis tag
#define JENIS_TAG_MIFARE_1K_4BYTE_ID                  0x01
#define JENIS_TAG_MIFARE_1K_7BYTE_ID                  0x02
#define JENIS_TAG_MIFARE_ULTRALIGHT_NATG203_7BYTE_ID  0x03
#define JENIS_TAG_MIFARE_4K_4BYTE_ID                  0x04
#define JENIS_TAG_MIFARE_4K_7BYTE_ID                  0x05
#define JENIS_TAG_MIFARE_DESFIRE_7BYTE_ID             0x06
#define JENIS_TAG_LAINNYA                             0x0A

// Jumlah byte ID tag, cocokan berdasarkan jenis tag yang digunakan
// dengan merujuk definisi jenis tag, pada contoh kali ini
// di demonstrasikan untuk jenis tag MIFARE_1K_4BYTE_ID sehingga
// jumlah byte tersimpan untuk ID adalah 4 byte
#define JUMLAH_BYTE_ID                                4

// Record database kolom ID tag
// Untuk tag jenis MIFARE 1k, panjang ID = 4 byte
// Maksimal
unsigned char ID_Tersimpan[JUMLAH_TAG_TERSIMPAN][JUMLAH_BYTE_ID] = {
    {0x82,0xE2,0xEE,0xBE},
    {/* Masukan ID tag lain*/},
    {/* Masukan ID tag lain*/},
    {/* Masukan ID tag lain*/},
    {/* Masukan ID tag lain*/},
    {/* Masukan ID tag lain*/},
    {/* Masukan ID tag lain*/},
    {/* Masukan ID tag lain*/},
    {/* Masukan ID tag lain*/}
};

// Record database kolom pemilik tag
// Maksimal panjang karakter untuk setiap nama
// ditentukan pada definisi PANJANG_NAMA_MAKS
char Nama_Tersimpan[JUMLAH_TAG_TERSIMPAN][PANJANG_NAMA_MAKS] = {
    {"SMKTeknosa"},
    {/* Masukan pemilik tag lain*/},
    {/* Masukan pemilik tag lain*/},
    {/* Masukan pemilik tag lain*/},
    {/* Masukan pemilik tag lain*/},
    {/* Masukan pemilik tag lain*/},
    {/* Masukan pemilik tag lain*/},
    {/* Masukan pemilik tag lain*/},
    {/* Masukan pemilik tag lain*/}
};

// Deklarasi pin-pin yang digunakan untuk LCD 16x2
LiquidCrystal lcd(3, 4, 5, 6, 7, 8);

bool deteksiBaru=1;

void setup()
{
    // Insialisai komunikasi Serial dengan PC
    Serial.begin(9600);
    // Inisialisasi RFID reader
    RFIDshield.init();
    // Inisialisasi LCD aplhanumeric 16 kolom x 2 baris
    lcd.begin(16, 2);
    // Gunakan Pin 3 sebgai input deteksi adanya tag
    pinMode(DETEKSI_TAG, INPUT);
    // Gunakan pin 11 sebagai output buzzer
    pinMode(BUZZER, OUTPUT);
    //lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" A-UDIK Absensi");
    delay(2000);
}

void loop()
{
    unsigned char x, y, z=0;
    unsigned char statusPilih;
    unsigned char i;
    int panjangID;
    int indexIDcocok;

    if (deteksiBaru == true) {
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("Dekatkan Tag");
      deteksiBaru=false; 
    }
    // Jika reader mendeteksi tag
    while (digitalRead(DETEKSI_TAG) == 0) {
        Serial.println("Kartu terdeteksi!");
        Serial.println("Mengirim perintah SELECT");
        // Mengirim perintah pilih (SELECT)
        statusPilih = RFIDshield.select();

        switch (statusPilih) {
          case STATUS_PEMBACAAN_OK:
              Serial.println("Pembacaan berhasil");
              break;
          case STATUS_TIDAK_ADA_TAG:
              Serial.println("Tidak ada tag yang kompatibel!");
              break;
          case STATUS_TUMBUKAN_TAG:
              Serial.println("Terjadi tumbukan tag");
              break;
        }
        if (statusPilih == STATUS_PEMBACAAN_OK) { // Jika operasi SELECT berhasil
            // Cari tahu jenis tag dan panjang ID berdasarkan jenis tag,
            // informasi jenis tag ada pada index terakhir di
            // struktur data pembacaan RFID (Datasheet SL018 halaman 9)
            // untuk mengetahui index terakhir, maka kita gunakan
            // informasi yang tersimpan pada kolom Len
            unsigned char jenisTag = 
              RFIDshield.cardata[(RFIDshield.cardata[0])];
            Serial.print("Jenis Tag: ");
            switch (jenisTag) {
              case JENIS_TAG_MIFARE_1K_4BYTE_ID:
                  Serial.println("Mifare 1K 4Byte UID");
                  panjangID = 4;
                  break;
              case JENIS_TAG_MIFARE_4K_4BYTE_ID:
                  Serial.println("Mifare 4K 4Byte UID");
                  panjangID = 4;
                  break;
              case JENIS_TAG_MIFARE_1K_7BYTE_ID:
                  Serial.println("Mifare 1K 7Byte UID");
                  panjangID = 7;
                  break;
              case JENIS_TAG_MIFARE_ULTRALIGHT_NATG203_7BYTE_ID:
                  Serial.println("Mifare UltraLight & NATG203 7Byte UID");
                  panjangID = 7;
                  break;
              case JENIS_TAG_MIFARE_4K_7BYTE_ID:
                  Serial.println("Mifare 4K 7Byte UID");
                  panjangID = 7;
                  break;
              case JENIS_TAG_MIFARE_DESFIRE_7BYTE_ID:
                  Serial.println("Mifare Desfire 4Byte UID");
                  panjangID = 4;
                  break;
            }
            digitalWrite(BUZZER,1);
            delay(100);
            digitalWrite(BUZZER,0);
            // Kirim ID tag melalui Serial PC
            // ID tag ada di index 3 pada struktur data hasil
            // pembacaan RFID reader
            Serial.print("ID: ");
            // Cetak ID di LCD
            lcd.clear();
            lcd.print("ID: ");
            for (i=3; i<panjangID+3; i++) {
                lcd.print(RFIDshield.cardata[i], HEX);
                lcd.print(" ");
                Serial.print(RFIDshield.cardata[i], HEX);
                Serial.print(" ");
            }
            Serial.println("");
            delay(500);
            if (panjangID == 4) {
                Serial.println("Mencocokan ID tag dengan record database");
                // Cocokan tag dengan database, ID tag dimulai dari index 3
                indexIDcocok = cocokanTag(&RFIDshield.cardata[3],JUMLAH_BYTE_ID);
                lcd.setCursor(0,1);
                if (indexIDcocok >= 0) {
                    for (i=0; i<3; i++) {
                      digitalWrite(BUZZER,1);
                      delay(55);
                      digitalWrite(BUZZER,0); 
                      delay(55);
                    }
                    digitalWrite(BUZZER,0); 
                    Serial.print("Pemilik tag: ");
                    Serial.println(Nama_Tersimpan[indexIDcocok]);
                    lcd.print("Nama: ");
                    lcd.print(Nama_Tersimpan[indexIDcocok]);
                } else {
                    digitalWrite(BUZZER,1);
                    delay(400);
                    digitalWrite(BUZZER,0);
                    Serial.println("Pemilik tag tidak ditemukan dalam database");
                    lcd.print("ID tidak cocok!");
                }
                Serial.println();
            } else
                Serial.println("Panjang ID bukan 4 byte, modifikasilah database berdasarkan panjang ID tag");
        }
        // Tunggu hingga tag tidak terdeteksi lagi
        while (digitalRead(DETEKSI_TAG) == 0);
        delay(3000);
        deteksiBaru=true;
        //Serial.println("");
    }
}

/** @brief Mencocokan tag yang terdeteksi dengan database yang tersedia
 *
 * @param id unsigned char* Pointer menunjuk ke ID tag yang terdeteksi
 * @param panjangID unsigned char Panjang ID dalam satuan byte
 * @return int Index record database, -1 jika tidak ditemukan
 *
 */
int cocokanTag(unsigned char *id, unsigned char panjangID)
{
    unsigned char i,j;
    bool cocok=true;

    for (i=0; i<JUMLAH_TAG_TERSIMPAN; i++) {
        cocok=true;

        for (j=0; j<panjangID; j++) {
            if (id[j] != ID_Tersimpan[i][j]) {
                cocok=false;
                break;
            }
        }
        if (cocok == true) {
            return i;
        }
    }
    return -1;
}
