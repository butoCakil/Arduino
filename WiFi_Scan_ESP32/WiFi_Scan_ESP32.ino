#include <WiFi.h>
#include <WiFiScan.h>
#include <vector>

struct WifiInfo {
  String ssid;
  int rssi;
  String security;
};

std::vector<WifiInfo> wifiList;

void printWifiList() {
  Serial.println("WiFi networks found:");
  for (const auto& wifi : wifiList) {
    Serial.print("SSID: ");
    Serial.print(wifi.ssid);
    Serial.print(", RSSI: ");
    Serial.print(wifi.rssi);
    Serial.print(" dBm, Security: ");
    Serial.println(wifi.security);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Scanning WiFi...");
}

void loop() {
  // Bersihkan data WiFi sebelum pemindaian baru
  wifiList.clear();

  // Tunggu hingga pemindaian WiFi selesai
  int networks = WiFi.scanNetworks();

  // Tambahkan informasi WiFi ke dalam vektor
  for (int i = 0; i < networks; ++i) {
    WifiInfo wifi;
    wifi.ssid = WiFi.SSID(i);
    wifi.rssi = WiFi.RSSI(i);
    switch (WiFi.encryptionType(i)) {
      case WIFI_AUTH_OPEN:
        wifi.security = "Open";
        break;
      case WIFI_AUTH_WEP:
        wifi.security = "WEP";
        break;
      case WIFI_AUTH_WPA_PSK:
      case WIFI_AUTH_WPA2_PSK:
      case WIFI_AUTH_WPA_WPA2_PSK:
        wifi.security = "WPA/WPA2";
        break;
      default:
        wifi.security = "Unknown";
        break;
    }
    wifiList.push_back(wifi);
  }

  // Tampilkan hasil pemindaian
  if (networks == 0) {
    Serial.println("No WiFi networks found");
  } else {
    printWifiList();
  }

  // Tunggu beberapa detik sebelum melakukan pemindaian berikutnya
  delay(5000);
}
