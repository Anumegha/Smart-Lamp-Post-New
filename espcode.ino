

#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <DHT.h>  // Including library for dht
#include <ThingSpeak.h>
#include "ThingSpeak.h"

//SoftwareSerial espSerial(4,5);
char ssid[] = "";   // your network SSID (name) 
char pass[] = "";   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;
 
unsigned long myChannelNumber = 1413908;
const char * myWriteAPIKey = "FTO7U3IW259EGE1G";
String myString; // complete message from arduino, which consistors of snesors data
char rdata; // received charactors
int firstVal, secondVal,thirdVal,val,bv,ba,sv,sa;

void setup() {
Serial.begin(115200);  // Initialize serial 
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client); 
}


void loop() { 

  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      //WiFi.begin(ssid);
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
  if (Serial.available() > 0 ) 
  {
    rdata = Serial.read(); 
    myString = myString+ rdata; 
   // Serial.print(rdata);
    if( rdata == '\n')
    {
   //  Serial.println(myString); 
  // Serial.println("fahad");
// new code
String l = getValue(myString, ',', 0);
String m = getValue(myString, ',', 1);
String n = getValue(myString, ',', 2); 
String o = getValue(myString, ',', 3);
String a = getValue(myString, ',', 4); 
String b = getValue(myString, ',', 5); 
String c = getValue(myString, ',', 6); 
String d = getValue(myString, ',', 7);  
 
 
firstVal = l.toInt();
secondVal = m.toInt();
thirdVal = n.toInt();
val = o.toInt();
bv = a.toInt();
ba = b.toInt();
sv = c.toInt();
sa = d.toInt();
 
ThingSpeak.setField(1, firstVal);
ThingSpeak.setField(2, secondVal);
ThingSpeak.setField(3, thirdVal);
ThingSpeak.setField(4, val);
ThingSpeak.setField(5, bv);
ThingSpeak.setField(6, ba);
ThingSpeak.setField(7, sv);
ThingSpeak.setField(8, sa);
 
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  delay(500000); // Wait 20 seconds to update the channel again
    }
  }
}

 
String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;
 
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
