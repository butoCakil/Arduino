

//www.nyebarilmu.com

//deklarasi pin yang digunakan
int A=8; //pin 8 - pin 11
int B=9;
int C=10;
int D=11; 
// set berapa kali maju & mundur
int maju=2;
int mundur=4;
//kecepatan puutar mS
int kecepatan=1000;

void setup(){
  //di deklarasikan sebagai output
pinMode(A,OUTPUT); pinMode(B,OUTPUT);
pinMode(C,OUTPUT); pinMode(D,OUTPUT);
}

//0 0 1 1
void pergerakan_1(){
digitalWrite(A,00); 
digitalWrite(D,1);
digitalWrite(B,00);
digitalWrite(C,1); 
}
//1 0 0 1
void pergerakan_2(){
digitalWrite(A,1); 
digitalWrite(D,1);
digitalWrite(B,00);
digitalWrite(C,00); 
}

//1 1 0 0
void pergerakan_3(){
digitalWrite(A,1); 
digitalWrite(D,00);
digitalWrite(B,1);
digitalWrite(C,00);
}

//0 1 1 0
void pergerakan_4(){
digitalWrite(A,00); 
digitalWrite(D,00);
digitalWrite(B,1);
digitalWrite(C,1);
}

void pergerakanMaju(){
//pergerakan dari 00, 90', 180', 270', 360'

    pergerakan_1(); delay(kecepatan);  //90'
    pergerakan_2(); delay(kecepatan);  //180'
    pergerakan_3(); delay(kecepatan);  //270'
    pergerakan_4(); delay(kecepatan);  //360'
  }
void pergerakanMundur(){
    pergerakan_1(); delay(kecepatan);  //
    pergerakan_4(); delay(kecepatan);  //
    pergerakan_3(); delay(kecepatan);  //
    pergerakan_2(); delay(kecepatan);  //
  }

void loop(){
  for(int i=0; i<maju; i++){
    pergerakanMaju();
  }
  for(int j=0; j<mundur; j++){
    pergerakanMundur();
  }
}
