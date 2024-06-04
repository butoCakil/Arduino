#include <ESP8266WiFi.h>

const char *ssid = "mqtt";
const char *password = "sijipingwolu";

#define RELAY_PIN D1

WiFiServer server(80);
void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  
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
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('r');
  Serial.println(request);
  client.flush();

  int value = HIGH;  //initially off
  if (request.indexOf("/kunci=OFF") != -1) {
    digitalWrite(RELAY_PIN, HIGH);
    value = HIGH;
  }
  if (request.indexOf("/kunci=ON") != -1) {
    digitalWrite(RELAY_PIN, LOW);
    value = LOW;
  }

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"stylesheet\" href=\"https://www.w3schools.com/w3css/4/w3.css\">");
  client.println("<style>p.padding{padding-left: 0.4cm;}p{color: black;}cred{color: red}cgreen{color: green}</style>");

  client.print("<br><p class=\"padding\">Kunci Pintu Saya : ");
  //High=off
  //Low=on

  if (value == HIGH) {
    client.print("<cred>Terkunci</cred>");
  } else {
    client.print("<cgreen>Terbuka<cgreen></p>");
  }
  client.println("<div class=\"w3-container\">");
  client.println("<br>");
  client.println("<a href=\"/kunci=ON\"\"><button class=\"w3-btn w3-ripple w3-green\">BUKA </button></a>");
  client.println("<a href=\"/kunci=OFF\"\"><button class=\"w3-btn w3-ripple w3-red\">KUNCI </button></a><br />");
  client.println("</div>");
  client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}
