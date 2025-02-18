#include "WiFi.h" // Use the WiFi library for ESP32

const int RSSI_MAX = -50;  // define maximum strength of signal in dBm
const int RSSI_MIN = -100; // define minimum strength of signal in dBm

const int displayEnc = 1; // set to 1 to display Encryption or 0 not to display

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Wifi Signal Scan");
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);

  Serial.println("Setup done");
}

void loop() {
  Serial.println("Wifi scan started");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("Wifi scan ended");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(") ");
      Serial.print(WiFi.SSID(i)); // SSID
      
      Serial.print(" ch:");  
      Serial.print(WiFi.channel(i)); // display channel
      Serial.print(" ");     
                  
      Serial.print(WiFi.RSSI(i)); // Signal strength in dBm  
      Serial.print("dBm (");
      
      Serial.print(dBmtoPercentage(WiFi.RSSI(i))); // Signal strength in %  
      Serial.print("% )");

      Serial.print(" MAC:");
      Serial.print(WiFi.BSSIDstr(i)); // MAC address

      if (displayEnc) {
        Serial.print(" Encryption:");
        Serial.println(encType(i));
      }

      // Check for the specific network
      if (WiFi.SSID(i) == "INTRUKTUR-TAV-2.4G") {
        Serial.println("");
        Serial.println("");
        Serial.println("KETEMU : INTRUKTUR-TAV-2.4G");
        Serial.print("kekuatan sinyal : ");
        Serial.print(WiFi.RSSI(i));
        Serial.print(" dB | ");
        Serial.print(dBmtoPercentage(WiFi.RSSI(i)));
        Serial.print(" %");
        Serial.println(""); 
        Serial.println("");
      }
      
      delay(10);
    }
  }
  Serial.println("");

  // Wait a bit before scanning again
  delay(5000);
  WiFi.scanDelete();  
}

// Function to return encryption type as a String
String encType(int id) {
  String type;
  wifi_auth_mode_t encryption = WiFi.encryptionType(id); // Use the ESP32 encryption type
  
  switch (encryption) {
    case WIFI_AUTH_WEP:
      type = "WEP";
      break;
    case WIFI_AUTH_WPA_PSK:
      type = "WPA / PSK";
      break;
    case WIFI_AUTH_WPA2_PSK:
      type = "WPA2 / PSK";
      break;
    case WIFI_AUTH_WPA_WPA2_PSK:
      type = "WPA / WPA2 / PSK";
      break;
    case WIFI_AUTH_OPEN:
      type = "<<OPEN>>";
      break;
    default:
      type = "Unknown";
      break;
  }

  return type;
}

// Approximate percentage calculation of RSSI
int dBmtoPercentage(int dBm) {
  int quality;
  if (dBm <= RSSI_MIN) {
    quality = 0;
  } else if (dBm >= RSSI_MAX) {
    quality = 100;
  } else {
    quality = 2 * (dBm + 100);
  }

  return quality;
}
