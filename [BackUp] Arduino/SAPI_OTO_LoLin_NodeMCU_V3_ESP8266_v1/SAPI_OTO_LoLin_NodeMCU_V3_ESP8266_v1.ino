#include <ESP8266WiFi.h>
const char* ssid = "ESEpro";
const char* password = "sijitekanwolu";
WiFiServer server(80);
//const char* host = "192.168.0.130";
// Konfigurasi PIN
int wadahPakan = 2, stateWadah = HIGH; // PIN UNTUK Wadah Pakan
int sensorPakan = 5; // PIN untuk Input Sensor Prox. D1
int konveyor = 4, stateKonveyor = HIGH; //PIN untuk motor konveyor
int pendorong = 0, statePendorong = HIGH; //PIN untuk pendorng pakan 
int i, step1, step2, step3;

unsigned long preMil1 = 0;
unsigned long preMil2 = 0;
unsigned long preMil3 = 0;
const long interval1 = 1000;
const long interval2 = 5000;
const long interval3 = 3000;
unsigned long Milliskrg;

void setup() {
    Serial.begin(115200);
    delay(10);

    // konfigurasi pin
    pinMode(wadahPakan, OUTPUT);
    pinMode(konveyor, OUTPUT);
    pinMode(pendorong, OUTPUT);
    pinMode(sensorPakan, INPUT);
    pinMode(sensorPakan, INPUT);
    digitalWrite(wadahPakan, HIGH);

    // Connect to WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    // Start the server
    server.begin();
    Serial.println("Server started");

    // Print the IP address
    Serial.print("Use this URL to connect: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");

}

void loop() {
  
 // Check if a client has connected
 WiFiClient client = server.available();
 if (!client) {
 return;
 }

// Wait until the client sends some data
 Serial.println("new client");
 while(!client.available()){
 delay(1);
 }

// Read the first line of the request
 String request = client.readStringUntil('r');
 Serial.println(request);
 client.flush();

// Match the request
 //Lolin nodeMCU has inverted the LED.
 //Low level turns on the led
 //while High level turns it off

int statusnya = HIGH; //variabel status level wadah pakan
int value = HIGH; //variabel setting manual

 if (request.indexOf("/WADAH=OFF") != -1) {
   digitalWrite(wadahPakan, HIGH);
   value = HIGH;
 }
 if (request.indexOf("/WADAH=ON") != -1) {
   digitalWrite(wadahPakan, LOW);
   value = LOW;
 }

 if(wadahPakan == LOW || (request.indexOf("/RUN") != -1)){
    value = LOW;
  } else {value = HIGH;}

//    // input sensor
byte valor = digitalRead(sensorPakan); 
 if (valor == HIGH) {
//memnta status wadah pakan
      if(request.indexOf("/CEKSTATUS") != -1){
          statusnya = HIGH;
        }
 }
else {
//memnta status wadah pakan
      if(request.indexOf("/CEKSTATUS") != -1){
          statusnya = LOW;
        } 
 }

//Menjalankan Siklus Pakan
//----------------------------------------//

 if (request.indexOf("/RUN") != -1) {
    i = 1;
    stateWadah = LOW;
    Milliskrg = millis();
//    stateKonveyor = LOW;
//    statePendorong = LOW;
  } else {preMil1 = millis();}
    //makanan turun : wadah pakan OPEN
    if(Milliskrg - preMil1 >= interval1) {
        preMil1 = Milliskrg;
        stateWadah = !stateWadah;
        if(i==1){
          digitalWrite(wadahPakan, stateWadah); 
          i++;
          stateKonveyor = LOW;
          }
      }
//      digitalWrite(wadahPakan, stateWadah);
//      step1 = HIGH;
//      delay(1000);
      //wadah pakan: CLOSE
//      digitalWrite(wadahPakan, stateWadah);
//      step1 = LOW;
      
      //konveyor berjalan dalam sekian detik : Konveyor ON ( .... detik)

      digitalWrite(konveyor, stateWadah);
//      step2 = HIGH;
//      delay(5000);
//      //konveyor OFF
//      digitalWrite(konveyor, stateWadah);
//      step2 = LOW;
      
      //Pendorong: AKTIF
//      for(i=6000;i<9000;i++){digitalWrite(pendorong, LOW);}
//      digitalWrite(pendorong, stateWadah);
//      step3 = HIGH;
//      delay(3000);
//      //Berhenti: SEMUA OFF
//      digitalWrite(pendorong, stateWadah);
//      step3 = LOW;

// Return the response
 client.println("HTTP/1.1 200 OK");
 client.println("Content-Type: text/html");
 client.println(""); 
 client.println("<!DOCTYPE HTML>");
 client.println("<html>");
 client.println("<h1>PAKAN SAPI OTOMATIS Berbasis IoT</h1><br>");
 client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
 client.println("<link rel=\"stylesheet\" href=\"https://www.w3schools.com/w3css/4/w3.css\">");
 client.println("<style>p.padding{padding-left: 0.4cm;}p{color: black;}cred{color: red}cgreen{color: green}</style>");
 client.println("<div class=\"w3-container\">");    
 client.println("<a href=\"/\"\"><button class=\"w3-btn w3-ripple w3-black\">Home </button></a>");
 client.println("<br>");
 client.println("<a href=\"/CEKSTATUS\"\"><button class=\"w3-btn w3-ripple w3-blue\">Cek Status </button>");
 client.println("<a href=\"/RUN\"\"><button class=\"w3-btn w3-ripple w3-yellow\">RUN </button></a>");
 client.print("<br><p class=\"padding\">Auto Setting (Status Wadah Pakan) : ");
 
if(statusnya == HIGH) {
        client.print("<cred>Warning</cred></p>");
    } else {
        client.print("<cgreen>Penuh<cgreen></p>");
    }
    
    client.println("</div>");
    client.println("</div>");
    
    client.println("<div class=\"w3-container\">");
    // menampilkan TOMBOL
    client.println("<br>");
    client.println("<a href=\"/WADAH=ON\"\"><button class=\"w3-btn w3-ripple w3-green\">Load Pakan </button></a>");
    client.println("<a href=\"/WADAH=OFF\"\"><button class=\"w3-btn w3-ripple w3-red\">Berhenti </button></a><br />");
    client.print("<p class=\"padding\">Manual Setting : ");
  
if(value == HIGH) {
        client.print("<cred>Berhenti</cred></p>");
    } else {
        client.print("<cgreen>Manual ON<cgreen></p>");
    }

//KEterangan tambahan
client.print("<p class=\"padding\">Wadah Pakan : ");
if(step1 == HIGH){
    client.print("<cred>ON</cred></p>");
  } else {
    client.print("<cgreen>OFF<cgreen></p>");
  }
client.print("<p class=\"padding\">Konveyor : ");
if(step2 == HIGH){
    client.print("<cred>ON</cred></p>");
  } else {
    client.print("<cgreen>OFF<cgreen></p>");
  }
client.print("<p class=\"padding\">Pendorong : ");
if(step3 == HIGH){
    client.print("<cred>ON</cred></p>");
  } else {
    client.print("<cgreen>OFF<cgreen></p>");
  }

    client.println("</div>");
    client.println("</html>");

    delay(1);
    Serial.println("Client disonnected");
    Serial.println("");
}
