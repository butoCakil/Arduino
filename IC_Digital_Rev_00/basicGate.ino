
// test gerbang logika AND, OR, NAND, NOR, X-OR, X-NOR
void test_GerbangDasar(int _tipe, int _jml_pin) {
  pinMode(pin(3, _jml_pin), INPUT_PULLUP);
  pinMode(pin(6, _jml_pin), INPUT_PULLUP);
  pinMode(pin(8, _jml_pin), INPUT_PULLUP);
  pinMode(pin(11, _jml_pin), INPUT_PULLUP);

  int _QA, _QB, _QC, _QD;
  int result = 0, i_test = 0;

  Serial.println(_tipe);
  Serial.println("A  B  Q");

  for (int i = 0; i                        < 2; i++) {
    for (int j = 0; j < 2; j++) {
      // A: 0 0
      digitalWrite(pin(1, _jml_pin), i);
      digitalWrite(pin(2, _jml_pin), j);

      // QA
      // int _QA = digitalRead(pin(3));
      int QA = validasiGerbang(i, j, _tipe);
      i_test++;

      Serial.print(i);
      Serial.print("  ");
      Serial.print(j);
      Serial.print("  ");
      Serial.println(QA);

      // B: 0 1
      digitalWrite(pin(4, _jml_pin), i);
      digitalWrite(pin(5, _jml_pin), j);

      // QB
      // int QB = digitalRead(pin(6));
      int QB = validasiGerbang(i, j, _tipe);
      i_test++;

      Serial.print(i);
      Serial.print("  ");
      Serial.print(j);
      Serial.print("  ");
      Serial.println(QB);

      // C: 1 0
      digitalWrite(pin(9, _jml_pin), i);
      digitalWrite(pin(10, _jml_pin), j);

      // QC
      // int QC = digitalRead(pin(8));
      int QC = validasiGerbang(i, j, _tipe);
      i_test++;

      Serial.print(i);
      Serial.print("  ");
      Serial.print(j);
      Serial.print("  ");
      Serial.println(QC);

      // D: 1 1
      digitalWrite(pin(12, _jml_pin), i);
      digitalWrite(pin(13, _jml_pin), j);

      // QD
      // int QD = digitalRead(pin(11));
      int QD = validasiGerbang(i, j, _tipe);
      i_test++;

      Serial.print(i);
      Serial.print("  ");
      Serial.print(j);
      Serial.print("  ");
      Serial.println(QD);

      if (QA == _QA) result++;
      if (QB == _QB) result++;
      if (QC == _QC) result++;
      if (QD == _QD) result++;

      Serial.println();

      delay(1000);
    }
  }

  Serial.println("Selesai");
  Serial.println("");
  Serial.print("Hasil: ");
  Serial.print(result);
  Serial.print("/");
  Serial.println(i_test);
  Serial.println();
  Serial.print("Hasil Test: ");

  int persen;
  persen = (result * 100) / i_test;
  Serial.print(persen);
  Serial.println("%");

  Serial.print("Kondisi: ");

  if (persen < 100) {
    Serial.println("TIDAK BAIK!");
  } else {
    Serial.println("BAIK!");
  }

  delay(10000);
}

int validasiGerbang(int a, int b, int _gerbang) {
  // Gerbang AND
  if (_gerbang == 7408) {
    if (a == 0 && b == 0) return 0;
    if (a == 0 && b == 1) return 0;
    if (a == 1 && b == 0) return 0;
    if (a == 1 && b == 1) return 1;
  }

  // Gerbang OR
  else if (_gerbang == 7432) {
    if (a == 0 && b == 0) return 0;
    if (a == 0 && b == 1) return 1;
    if (a == 1 && b == 0) return 1;
    if (a == 1 && b == 1) return 1;
  }

  // Gerbang NAND
  else if (_gerbang == 7400) {
    if (a == 0 && b == 0) return 1;
    if (a == 0 && b == 1) return 1;
    if (a == 1 && b == 0) return 1;
    if (a == 1 && b == 1) return 0;
  }
  // Gerbang NOR
  else if (_gerbang == 7402) {
    if (a == 0 && b == 0) return 1;
    if (a == 0 && b == 1) return 0;
    if (a == 1 && b == 0) return 0;
    if (a == 1 && b == 1) return 0;
  }
  // Gerbang X-OR
  else if (_gerbang == 7486) {
    if (a == 0 && b == 0) return 0;
    if (a == 0 && b == 1) return 1;
    if (a == 1 && b == 0) return 1;
    if (a == 1 && b == 1) return 0;
  }
  // Gerbang X-NOR
  else if (_gerbang == 74266) {
    if (a == 0 && b == 0) return 1;
    if (a == 0 && b == 1) return 0;
    if (a == 1 && b == 0) return 0;
    if (a == 1 && b == 1) return 1;
  }
}
