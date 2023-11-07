#include <WiFi.h>

// Ganti nama Wifi dan password
const char* ssid     = "INTRUKTUR-TAV-2.4G";    // nama WiFi
const char* password = "skanebabisa";           // wifi password

// Set web server port ke 80
WiFiServer server(80);

// Variable untuk request HTTP
String header;

// Variable untuk keadaan LED
String LED2 = "off";

// Variabel untuk pin LED
const int pinLED2 = 2;

// Waktu sekarang ()
unsigned long waktuSekarang = millis();
// Waktu sebelumnya
unsigned long waktuSebelumnya = 0; 
// Batas waktu
const long waktuHabis = 2000;

void setup() {
  // memulai komunikasi serial
  Serial.begin(115200);
  // Pengenalan (inisialisasi) variabel output sebagai output
  pinMode(pinLED2, OUTPUT);
  // Atur menjadi LOW
  digitalWrite(pinLED2, LOW);
  

  // Konek ke Wifi dan Password
  Serial.print("Menghubungkan ke ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Menampilkan IP ke serial monitor
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Membaca client yang masuk

  if (client) {                             // Jika ada Client Baru
    waktuSekarang = millis();
    waktuSebelumnya = waktuSekarang;
    Serial.println("Ketemu Client Baru.");  // Tampilkan ke serial monitor jika ada client baru
    String currentLine = "";  

    // Selama client terhubung
    while (client.connected() && waktuSekarang - waktuSebelumnya <= waktuHabis) { 
      waktuSekarang = millis();
      if (client.available()) {             // Jika ada data yang dibaca dari client
        char c = client.read();             // Baca data dari client
        Serial.write(c);                    // Tampilkan yang dibaca dari client ke serial monitor
        header += c;
        if (c == '\n') {                    // Jika ada karakter enter (Baris Baru ditemukan)
          // Jika data terakhir kosong, kirimkan reques HTML
          if (currentLine.length() == 0) {
            // konten HTML (halaman WEB)
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Keterangan di serial monitor dan kondisi LED
            if (header.indexOf("GET /2/on") >= 0) {
              Serial.println("LED 2 ON");
              LED2 = "on";
              digitalWrite(pinLED2, HIGH);
            } else if (header.indexOf("GET /2/off") >= 0) {
              Serial.println("LED 2 OFF");
              LED2 = "off";
              digitalWrite(pinLED2, LOW);
            } 
                    
            // Tampilkan halaman HTML
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // untuk mempervantik tampilan
            client.println("<style>html { font-family: Monaco; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #FF0000;}</style></head>");
            
            // Judul Halaman Web
            client.println("<body><h1>ESP32 Web Server</h1>");
            client.println("<body><h2>Latihan</h2>");
            
            // Tampilan Tombol 
            client.println("<p>Kondisi LED 2 - " + LED2 + "</p>");
            // Mengubah tampilan tombol sesuai kondisi LED
            if (LED2=="on") {
              client.println("<p><a href=\"/2/on\"><button class=\"button button2\">OFF</button></a></p>");           
            } else {
              client.println("<p><a href=\"/2/off\"><button class=\"button\">ON</button></a></p>");
            } 
                           
            client.println("</body></html>");
    
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {  // Jika tidak ada data apapun
          currentLine += c;      // tetap tampikan hasil bacaaan dari client
        }
      }
    }
    
    // bersihkan variable header
    header = "";
    // Tutup koneksi
    client.stop();
    Serial.println("Client diputus :'( hiks.");
    Serial.println("");
  }
 }
