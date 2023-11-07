#include "DHT.h"

// Deklarasi DHT
#define DHTPIN A1
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // NodeMCU Utility
  Serial.begin(9600);
  dht.begin();
}

void loop(){
  // DHT get temp dan humid
  int temp = dht.readTemperature(false);
  int humid = dht.readHumidity();

  Serial.println("=====================================================");
  Serial.println("\nSuhu: ");
  Serial.println(temp);
  Serial.println("\nKelembapan: ");
  Serial.println(humid);
  Serial.println("\n=====================================================");
  Serial.println("\n\n");

}
