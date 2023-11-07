void runningLED() {
  for (int i = 1; i <= 16; i++) {
    digitalWrite(pin(i, 16), HIGH);
    int ii;
    ii = i - 1;
    if (ii < 1) {
      ii = 16;
    }

    digitalWrite(pin(ii, 16), LOW);

    Serial.print("LED: ");
    Serial.println(i);

    delay(500);
  }
}

int pin(int _pin, int _jml_pin) {
  if (_jml_pin == 16) {
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
  } else if (_jml_pin == 14) {
    if (_pin == 1) return 21;
    else if (_pin == 2) return 19;
    else if (_pin == 3) return 18;
    else if (_pin == 4) return 5;
    else if (_pin == 5) return 17;
    else if (_pin == 6) return 16;
    else if (_pin == 7) return 4;
    else if (_pin == 8) return 14;
    else if (_pin == 9) return 27;
    else if (_pin == 10) return 26;
    else if (_pin == 11) return 25;     
    else if (_pin == 12) return 33;
    else if (_pin == 13) return 32;
    else if (_pin == 14) return 22;
  }
}