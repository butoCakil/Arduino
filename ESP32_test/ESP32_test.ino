#include <WiFi.h>

const char* ssid = "ASSEMBLY ONLY";      // nama WiFI
const char* pass = "onlyassemblytebos";  // password WiFi
WiFiServer server(80);                   // atur port di 80

String header;

void setup() {
  Serial.begin(115200);                  // boudrate COM serial
  pinMode(2, OUTPUT);                    // pin 2 (LED) digunakan sebagai output
  Serial.println("");                    // ENTER di tampilan serial monitor
  Serial.print("Menyambungkan Wifi..");  // komentar di serial monitor
  Serial.println(ssid);                  // tampilkan nama WIFI di serial monitor

  // Nonaktifkan penggunaan DHCP
  WiFi.mode(WIFI_STA);

  // Jika terhubung, coba perbarui alamat IP dengan alamat yang diinginkan
  IPAddress customIP(10, 16, 0, 123);  // Ganti ini dengan alamat IP yang diinginkan
  IPAddress subnet(255, 255, 255, 0);
  IPAddress gateway(10, 16, 0, 254);  // Ganti ini dengan gateway yang sesuai

  WiFi.config(customIP, gateway, subnet);

  WiFi.begin(ssid, pass);  // akses ke wifi

  // menunggu konek ke wifi
  while (((WiFi.status() != WL_CONNECTED))) {
    Serial.print(".");
    delay(300);
  }

  // tampilan di serial monitor setelah koneksi berhasil
  Serial.println("Koneksi Berhasil!");
  Serial.println("");
  // menampilkan Alamat IP ESP di serial monitor
  Serial.println("Alamat IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
  server.begin();
}

void loop() {
  // jika client ada
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Client Baru.");
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;

        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            if (header.indexOf("GET /H")) {
              Serial.println("LED 2 ON");
              client.println("<h1>LED MATI</h1>");
            } else if (header.indexOf("GET /L")) {
              Serial.println("LED 2 OFF");
              client.println("<h1>LED HIDUP</h1>");
            }
            client.println();

            client.println("<h1>");
            client.println("<a href=\"/H\">ON</a><br><br>");
            client.println("<a href=\"/L\">OFF</a>");
            client.println("</h1>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        if (currentLine.endsWith("GET /H")) {
          digitalWrite(2, HIGH);
        }

        if (currentLine.endsWith("GET /L")) {
          digitalWrite(2, LOW);
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client Disconnected..");
  }
}
