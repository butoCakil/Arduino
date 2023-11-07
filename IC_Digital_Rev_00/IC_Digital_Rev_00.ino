// Gerbang AND
#define tipeIC 7408
// Gerbang OR
// #define tipeIC 7432
// Gerbang NAND
// #define tipeIC 7400
// Gerbang NOR
// #define tipeIC 7402
// Gerbang X-OR
// #define tipeIC 7486
// Gerbang X-NOR
// #define tipeIC 74266

#define pinIC 14
// #define pin 16

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Mulai...");
  Serial.println();

  for (int i = 1; i <= 16; i++) {
    pinMode(pin(i, 16), OUTPUT);
    digitalWrite(pin(i, 16), LOW);
  }
}

void loop() {
  // test_GerbangDasar(tipeIC, pinIC);

  runningLED();
}
