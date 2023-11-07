#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
// Ganti dengan password Jaringan WiFi yang user miliki
const char* ssid = "Gas LPG";
const char* password = "12345678";
ESP8266WebServer server(80); //Inisialisasi server pada port 80
String page = "";

//Pin yang digunakan untuk mengendalikan LED
int LEDPin = D4; 
int LEDPin2 = D1;
void setup(void){
 //the HTML of the web page
 page = "<h1>Kelompok 5</h1><p><a href=\"LEDOn\"><button>ON</a>&nbsp;<a href=\"LEDOff\"><button>OFF</button></a></p><p><a href=\"LEDOn2\"><button>ON</a>&nbsp;<a href=\"LEDOff2\"><button>OFF</button></a></p>";
 //Inisialisasi pin D4 padam ketika NodeMCU pertama kali menyalautton>ON</butt
 pinMode(LEDPin, OUTPUT);
 digitalWrite(LEDPin, LOW);
 delay(1000);
 
 pinMode(LEDPin2, OUTPUT);
 digitalWrite(LEDPin2, LOW);
 delay(1000);
 
 Serial.begin(115200);
 WiFi.begin(ssid, password); //begin WiFi connection
 Serial.println("");
 // Wait for connection
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 }
 Serial.println("");
 Serial.print("Connected to ");
 Serial.println(ssid);
 Serial.print("IP address: ");
 Serial.println(WiFi.localIP());
 
 server.on("/", [](){
 server.send(200, "text/html", page);
 });
 
 server.on("/LEDOn", [](){
 server.send(200, "text/html", page);
 digitalWrite(LEDPin, HIGH);
 delay(1000);
 });
 
 server.on("/LEDOn2", [](){
 server.send(200, "text/html", page);
 digitalWrite(LEDPin2, HIGH);
 delay(1000);
 });
 
 server.on("/LEDOff", [](){
 server.send(200, "text/html", page);
 digitalWrite(LEDPin, LOW);
 delay(1000); 
 });
 
 server.on("/LEDOff2", [](){
 server.send(200, "text/html", page);
 digitalWrite(LEDPin2, LOW);
 delay(1000); 
 });
 
 
 server.begin();
 Serial.println("Web server started!");
 }
 
void loop(void){
 server.handleClient();
}
