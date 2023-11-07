byte sda = 10;
byte rst = 9;
#include<KRrfid.h>
using namespace std;

String dataBase[5]= {1186226, \
                     761124250, \
                     122520059, \
                     0814323734, \
                     8585182044128 \
                    };
void setup() {
  Serial.begin(9600);
  rfidBegin();
}

void loop() {
  getTAG();
  if (TAG != "") {
    //Serial.print("TAG :");
    Serial.println(TAG);
    TAG = "";
  }
}
