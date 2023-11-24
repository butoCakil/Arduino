#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "HTML.h"

// Konfigurasi WiFi
const char *ssid = "Mencari Korban Hack";
const char *password = "";

String ssidNew = "", passNew, hotspotNew, nodeviceNew, hostNew;

ESP8266WebServer server(80);

//== Rountin dieksekusi saat browser dibuka / page dipanggil ==//
void handleRoot(){
  server.send(200, "text/html", index_html);
}

void handleForm(){
  ssidNew = server.arg("ssidNew");
  passNew = server.arg("passNew");
  hotspotNew = server.arg("hotspot");
  nodeviceNew = server.arg("nodevice");
  hostNew = server.arg("host");

  Serial.println("");
  Serial.println("SSID: ");
  Serial.println(ssidNew);
  Serial.println("PASS: ");
  Serial.println(passNew);
  Serial.println("HOTSPOT: ");
  Serial.println(hotspotNew);
  Serial.println("nodevice: ");
  Serial.println(nodeviceNew);
  Serial.println("HOST: ");
  Serial.println(hostNew);
  Serial.println("");

  server.send(200, "text/html", sukses_html);
  delay(2000);    // Agar perangkat dapat mengirimkan data sebelum disconnect

  WiFi.softAPdisconnect(true);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssidNew, passNew);

  // Tunggu sampai terhubung
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Terhubung Ke Jaringan");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(IP);

  server.on("/", handleRoot); // Routine untuk menghandle homepage
  server.on("/action_page", handleForm);

  server.begin(); // Mulai server
  delay(1000);
}

void loop() {
  if(ssidNew == ""){
    server.handleClient();
  } else {
    Serial.println("Yeay Terhubung!!!!");
    delay(1000);
  }
}
