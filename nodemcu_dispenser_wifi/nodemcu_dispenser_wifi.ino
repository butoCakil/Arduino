#include <ESP8266WiFi.h>


const char* ssid     = "ESEpro";
const char* password = "masihsama";

const char* host = "presensi.smknbansari.sch.id";

WiFiClient client;
const int httpPort = 80;
String url;

unsigned long timeout;
  
void setup() {
  Serial.begin(9600);
  delay(10);

  pinMode(D0, OUTPUT);
  digitalWrite(D0, LOW);
  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void loop() {
  
  Serial.print("connecting to ");
  Serial.println(host);

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    //return;
  }

// We now create a URI for the request
  url = "/app/feedback.php";
  
  Serial.print("Requesting URL: ");
  Serial.println(url);

// This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

// Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
    
    if(client.find("OK")){
      digitalWrite(D0,HIGH);     //lampu on
      Serial.println("Kran ON");
    }else if(client.find("MIN")){  
      digitalWrite(D0,LOW);    //lampu off
      Serial.println("Kran OFF, Warning!!");
    }else{
      digitalWrite(D0,LOW);    //lampu off
      Serial.println("Kran OFF, Tidak ada permintaan");
    }
  }

  Serial.println();
  Serial.println("closing connection");
  Serial.println();
  
}
