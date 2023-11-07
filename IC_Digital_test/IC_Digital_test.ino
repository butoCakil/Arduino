// #define pin1 21
// #define pin2 19
// #define pin3 18
// #define pin4 5
// #define pin5 17
// #define pin6 16
// #define pin7 4
// #define pin8 15
// #define pin9 13
// #define pin10 14
// #define pin11 27
// #define pin12 26
// #define pin13 25
// #define pin14 33
// #define pin15 32
// #define pin16 22
int pin(int _pin) {
  if (_pin == 1) return 21;
  else if (_pin == 2) return 19;
  else if (_pin == 3) return 18;
  else if (_pin == 4) return 5;
  else if (_pin == 5) return 17;
  else if (_pin == 6) return 16;
  else if (_pin == 7) return 4;
  else if (_pin == 8) return 15;
  else if (_pin == 9) return 13;
  else if (_pin == 10) return 14;
  else if (_pin == 11) return 27;
  else if (_pin == 12) return 26;
  else if (_pin == 13) return 25;
  else if (_pin == 14) return 33;
  else if (_pin == 15) return 32;
  else if (_pin == 16) return 22;
}

void setup() {
  Serial.begin(9600);
  Serial.println("Mulai...");
  Serial.println("");

  for (int i = 1; i <= 16; i++) {
    pinMode(pin(i), OUTPUT);
    digitalWrite(pin(i), LOW);
  }
}

void loop() {
  for (int i = 1; i <= 16; i++) {
    digitalWrite(pin(i), HIGH);
    int ii;
    ii = i - 1;
    if (ii < 1) {
      ii = 16;
    }

    digitalWrite(pin(ii), LOW);

    Serial.print("LED: ");
    Serial.println(i);

    delay(1000);
  }

  for (int i = 1; i <= 16; i++) {
    digitalWrite(pin(i), HIGH);
  }
  
  delay(10000);
}
