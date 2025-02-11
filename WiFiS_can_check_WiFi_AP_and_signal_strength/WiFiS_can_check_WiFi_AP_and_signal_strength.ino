#include "ESP8266WiFi.h"

const int RSSI_MAX =-50;// define maximum strength of signal in dBm
const int RSSI_MIN =-100;// define minimum strength of signal in dBm

const int displayEnc=1;// set to 1 to dispaly Encryption or 0 not to display

void setup() {
  Serial.begin(115200);
  Serial.println("Robojax Wifi Signal Scan");
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
      Serial.print(WiFi.SSID(i));// SSID
      
      Serial.print(" ch:");  
      Serial.print(WiFi.channel(i));// display channel
      Serial.print(" ");     
                  
      Serial.print(WiFi.RSSI(i));//Signal strength in dBm  
      Serial.print("dBm (");

      
      Serial.print(dBmtoPercentage(WiFi.RSSI(i)));//Signal strength in %  
      Serial.print("% )");

      Serial.print(" MAC:");
      Serial.print(WiFi.BSSIDstr(i)  );//MAC address  (Basic Service Set Identification) 
            
    
      if(WiFi.isHidden(i) ){
       Serial.print(" <<Hidden>> ");           
      }
     if(displayEnc)
     {
        Serial.print(" Encryption:");      
        Serial.println(encType(i));    
      }// if displayEnc    


      if(WiFi.SSID(i) == "INTRUKTUR-TAV-2.4G"){
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
}// loop


/*
 * @brief returns the encryption type as String
 * @param "id" network ID integer
 * @return String text of network type
 *    Usage: encType(i)
 *    //where i is id of scanned network
 * Written by Ahmad Shamshiri on August 10, 2019 at 17:33
 * in Ajax, Ontario, Canada
 * www.Robojax.com 
 */
 
String encType(int id){

String type;
  if(WiFi.encryptionType(id) == ENC_TYPE_WEP){
    type=" WEP";
  }else if(WiFi.encryptionType(id) == ENC_TYPE_TKIP){
    type="WPA / PSK";    
  }else if(WiFi.encryptionType(id) == ENC_TYPE_CCMP){
    type="WPA2 / PSK";    
  }else if(WiFi.encryptionType(id) == ENC_TYPE_AUTO){
    type="WPA / WPA2 / PSK";    
  }else if(WiFi.encryptionType(id) == ENC_TYPE_NONE){
    type="<<OPEN>>";    
  }
  return type;
//1:  ENC_TYPE_WEP – WEP
//2 : ENC_TYPE_TKIP – WPA / PSK
//4 : ENC_TYPE_CCMP – WPA2 / PSK
//7 : ENC_TYPE_NONE – open network
//8 : ENC_TYPE_AUTO – WPA / WPA2 / PSK
}

/*
 * Written by Ahmad Shamshiri
  * with lots of research, this sources was used:
 * https://support.randomsolutions.nl/827069-Best-dBm-Values-for-Wifi 
 * This is approximate percentage calculation of RSSI
 * Wifi Signal Strength Calculation
 * Written Aug 08, 2019 at 21:45 in Ajax, Ontario, Canada
 */

int dBmtoPercentage(int dBm)
{
  int quality;
    if(dBm <= RSSI_MIN)
    {
        quality = 0;
    }
    else if(dBm >= RSSI_MAX)
    {  
        quality = 100;
    }
    else
    {
        quality = 2 * (dBm + 100);
   }

     return quality;
}//dBmtoPercentage 


// 5) INTRUKTUR-TAV-2.4G ch:11 -63dBm (74% ) MAC:2C:C8:1B:D1:30:67 Encryption:WPA / WPA2 / PSK
