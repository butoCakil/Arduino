#include "WiFi.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"

const char *ssid = "INTRUKTUR-TAV-2.4G";
const char *password = "skanebabisa1";

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);


  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());

  // if (!SPIFFS.begin()) {
  //   Serial.println("An Error has occurred while mounting SPIFFS");
  //   return;
  // }

  // server.on("/index", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send(SPIFFS, "/index.html", "text/html");
  // });

  // server.on("/src/bootstrap.bundle.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send(SPIFFS, "/src/bootstrap.bundle.min.js", "text/javascript");
  // });

  // server.on("/src/jquery-3.7.0.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send(SPIFFS, "/src/jquery-3.7.0.min.js", "text/javascript");
  // });

  // server.on("/src/min.bootstrap.css", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send(SPIFFS, "/src/min.bootstrap.css", "text/css");
  // });

  // server.begin();
}

void loop() {}