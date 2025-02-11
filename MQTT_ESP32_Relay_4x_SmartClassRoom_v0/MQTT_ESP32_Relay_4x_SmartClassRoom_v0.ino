/*
Status Baterai 
100 % = 4,2
90 %  = 4,1
80 %  = 4,0
70 %  = 3,9
60 %  = 3,8
50 %  = 3,7
40 %  = 3,6
30 %  = 3,5
20 %  = 3,4
18 %  = 3,3
16 %  = 3,2
14 %  = 3,1
12 %  = 3,0
10 %  = 2,9
8 %   = 2,8
5 %   = 2,7
2 %   = 2,6
0 %   = 2,5

y = (100/1.7) * (x - 2.5)

Persentase=((Tegangan − Tegangan Kosong) / (Tegangan Penuh − Tegangan Kosong))×100
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <MQTT.h>
#include <EEPROM.h>
#include <DHT.h>
#include <ACS712.h>

const char ssid[] = "Electronics";
const char pass[] = "tebos111";

WiFiClient net;
MQTTClient client;


#define DHT22Pin 27
#define led1 23

int relay[] = { 32, 33, 25, 26 };
int numRelay = sizeof(relay) / sizeof(relay[0]);

// Begin Sensor Arus Config
// Pin sensor ACS712
const int sensorPin = 35;

// Konfigurasi sensor ACS712
ACS712 sensor(sensorPin, 5.0, 1023, 100);

// Jumlah pembacaan untuk rata-rata bergerak
const int numReadings = 5;

// Array untuk menyimpan pembacaan terakhir
float readings[numReadings];

// Indeks untuk array pembacaan
int indexy = 0;
// end

// begin var data sub-pub
bool relayState[4];
bool ledState;
bool serialStatus = true;
unsigned long _delay;


// end

DHT dht(DHT22Pin, DHT22);

unsigned long lastMillis = 0;

const int eepromSize = 512;

void connect() {
  // Menghubungkan ke WiFi
  Serial.print("Cek WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  Serial.println("Sukses Terhubung ke WiFi");

  // Menghubungkan ke Broker
  Serial.print("Cek Broker");
  while (!client.connect("")) {  // Client ID boleh dikosongkan
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  Serial.println("Sukses Terhubung ke Server");

  client.subscribe("ben10/iot/#", 0);  // subcribe dengan QoS 1
}

void subscribeData(String &topic, String &data) {
  if (serialStatus) {
    Serial.print("Sub Topic: ");
    Serial.println(topic);
    Serial.print("Sub Data: ");
    Serial.println(data);
  }

  if (topic == "ben10/iot/input/led1") {  // Mengubah data pesan menjadi nilai boolean
    digitalWrite(led1, data.toInt());     // Kontrol LED 1 sesuai dengan pesan
    client.publish("ben10/iot/status/led1", String(ledState), true, 0);
  }

  if (topic == "ben10/iot/input/rl1") {
    relayState[0] = data.toInt();
    digitalWrite(relay[0], relayState[0]);
    client.publish("ben10/iot/status/rl1", String(relayState[0]), true, 0);
  }
  if (topic == "ben10/iot/input/rl2") {
    relayState[1] = data.toInt();
    digitalWrite(relay[1], relayState[1]);
    client.publish("ben10/iot/status/rl2", String(relayState[1]), true, 0);
  }

  if (topic == "ben10/iot/input/rl3") {
    relayState[2] = data.toInt();
    digitalWrite(relay[2], relayState[2]);
    client.publish("ben10/iot/status/rl3", String(relayState[2]), true, 0);
  }

  if (topic == "ben10/iot/input/rl4") {
    relayState[3] = data.toInt();
    digitalWrite(relay[3], relayState[3]);
    client.publish("ben10/iot/status/rl4", String(relayState[3]), true, 0);
  }

  if (topic == "ben10/iot/set/waktuPub") {
    if (serialStatus) {
      Serial.print("Set Update setiap: ");
      Serial.println(data.toInt());
      Serial.println();
    }

    _delay = data.toInt();
  }

  if (topic == "ben10/iot/set/serial") {
    Serial.print("Set Serial Monitor : ");
    Serial.println(data);
    if (data == "true") {
      serialStatus = true;
    } else {
      serialStatus = false;
    }

    if (serialStatus) {
      Serial.print("TRUE");
    } else {
      Serial.print("FALSE");
    }
    Serial.println();

    client.publish("ben10/iot/set/serialStatus", serialStatus ? "true" : "false", true, 0);
  }

  if (topic == "ben10/iot/set/eeprom") {
    saveToEEPROM();
  }
}

void publish() {
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();

  if (serialStatus) {
    Serial.print("Temperature : ");
    Serial.print(temp);
    Serial.print(" C ");

    Serial.print("Humidity : ");
    Serial.print(hum);
    Serial.println(" % ");
  }

  // Get WiFi signal quality
  int32_t rssi = WiFi.RSSI();

  if (serialStatus) {
    Serial.print("Signal strength (RSSI): ");
    Serial.print(rssi);
    Serial.println(" dBm");
  }

  client.publish("ben10/iot/status/led1", String(ledState), true, 0);

  client.publish("ben10/iot/status/rl1", String(relayState[0]), true, 0);
  client.publish("ben10/iot/status/rl2", String(relayState[1]), true, 0);
  client.publish("ben10/iot/status/rl3", String(relayState[2]), true, 0);
  client.publish("ben10/iot/status/rl4", String(relayState[3]), true, 0);

  client.publish("ben10/iot/sensor/kelembapan", String(hum), true, 0);
  client.publish("ben10/iot/sensor/suhu", String(temp), true, 0);
  client.publish("ben10/iot/wifi/rssi", String(rssi), true, 0);
  // client.publish("ben10/iot/set/waktuPub", String(_delay), true, 0);
  client.publish("ben10/iot/set/serialStatus", serialStatus ? "true" : "false", true, 0);
}

// Fungsi untuk menyimpan variabel-variabel ke EEPROM
void saveToEEPROM() {
  EEPROM.begin(512);
  uint16_t address = 0;

  // Simpan status relay
  if (serialStatus)
    Serial.println("Menyimpan relayState ke EEPROM: ");

  for (uint8_t i = 0; i < sizeof(relayState); i++) {
    EEPROM.write(address++, relayState[i]);
    if (serialStatus) {
      Serial.print("relayState[");
      Serial.print(i);
      Serial.print("]: ");
      Serial.print(relayState[i]);
      Serial.println();
    }
  }

  // Simpan status Serial
  Serial.print("Menyimpan serialStatus ke EEPROM: ");
  EEPROM.write(address++, serialStatus);
  Serial.print(serialStatus);
  Serial.println();

  // Simpan delay (dibagi menjadi byte-byte)
  if (serialStatus)
    Serial.print("Menyimpan _delay ke EEPROM: ");

  // Simpan delay
  EEPROM.write(address++, _delay);

  if (serialStatus) {
    Serial.print(_delay);
    Serial.println();
  }

  // Selesai
  EEPROM.commit();
  EEPROM.end();
}

// Fungsi untuk membaca variabel-variabel dari EEPROM
void loadFromEEPROM() {
  EEPROM.begin(512);
  uint16_t address = 0;

  // Baca status relay
  Serial.print("Membaca EEPROM relay: ");

  for (uint8_t i = 0; i < sizeof(relayState); i++) {
    relayState[i] = EEPROM.read(address++);
    Serial.print(relayState[i]);
  }

  // Baca status Serial
  Serial.println();
  Serial.print("Membaca EEPROM statusSerial: ");
  serialStatus = EEPROM.read(address++);
  Serial.print(serialStatus);
  Serial.println();

  // Baca delay (dibaca sebagai byte-byte dan disusun kembali)
  Serial.print("Membaca SSPROM _delay: ");

  // Baca delay
  _delay = EEPROM.read(address++);

  Serial.println(_delay);
  EEPROM.end();
}

void setup() {
  pinMode(led1, OUTPUT);

  for (int q = 0; q < numRelay; q++) {
    pinMode(relay[q], OUTPUT);
  }

  dht.begin();

  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  client.begin("broker.emqx.io", net);
  client.onMessage(subscribeData);
  connect();

  loadFromEEPROM();

  for (int r = 0; r < numRelay; r++) {
    digitalWrite(relay[r], relayState[r]);
    client.publish("ben10/iot/status/rl" + (r + 1), String(relayState[r]), true, 0);
  }
}

void reconnect() {
  // Loop sampai koneksi terhubung kembali
  while (!client.connected()) {
    if (client.connect("")) {  // Client ID boleh dikosongkan
      Serial.print("-");
      client.subscribe("ben10/iot/#", 0);  // subcribe dengan QoS 1
    } else {
      Serial.print("Gagal, kembali mencoba dalam 3 detik...");
      Serial.println();
      delay(3000);
    }

    Serial.println();
  }
}

void bacaArus() {
  // Baca nilai arus DC
  float rawCurrent = sensor.mA_DC();

  // Simpan nilai arus terbaru ke dalam array
  readings[indexy] = rawCurrent;

  // Hitung rata-rata nilai arus
  float averageCurrent = 0.0;
  for (int i = 0; i < numReadings; i++) {
    averageCurrent += readings[i];
  }
  averageCurrent /= numReadings;

  // Konversi nilai arus rata-rata ke mA
  int mA = averageCurrent / 1000;

  // Cetak nilai arus mA yang stabil ke serial monitor
  Serial.print(mA);
  Serial.println(" mA");

  client.publish("ben10/iot/sensor/arus1", String(mA), true, 0);

  // Perbarui indeks array
  indexy = (indexy + 1) % numReadings;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // publish a message roughly every second.
  if (millis() - lastMillis > ((_delay == 0 ? 10 : _delay) * 1000)) {
    lastMillis = millis();
    bacaArus();
    publish();
  }
}