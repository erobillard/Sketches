/*
 ESP8266 --> ThingSpeak Channel via MKR1000 Wi-Fi
 
 This sketch sends the Wi-Fi Signal Strength (RSSI) of an ESP8266 to a ThingSpeak
 channel using the ThingSpeak API (https://www.mathworks.com/help/thingspeak).
 
 Requirements:
 
   * ESP8266 Wi-Fi Device
   * Arduino 1.6.9+ IDE
   * Additional Boards URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json
   * Library: esp8266 by ESP8266 Community
 
 ThingSpeak Setup:
 
   * Sign Up for New User Account - https://thingspeak.com/users/sign_up
   * Create a new Channel by selecting Channels, My Channels, and then New Channel
   * Enable one field
   * Note the Channel ID and Write API Key
    
 Setup Wi-Fi:
  * Enter SSID
  * Enter Password
  
 Tutorial: http://nothans.com/measure-wi-fi-signal-levels-with-the-esp8266-and-thingspeak
   
 Created: Feb 1, 2017 by Hans Scharler (http://nothans.com)
 Updated: Feb 15, 2018 Eli Robillard. Converted POST to GET, added field2 to store SSID. 
*/

#include <ESP8266WiFi.h>

// Wi-Fi Settings
const char* ssid = "[WiFi SSID]"; // your wireless network name (SSID)
const char* password = "[WiFi Password]"; // your Wi-Fi network password

WiFiClient client;

// ThingSpeak Settings
const int channelID = 999; // Paste Channel ID
String writeAPIKey = "[APIKey]"; // write API key for your ThingSpeak Channel
const char* server = "api.thingspeak.com";
const int postingInterval = 20 * 1000; // post data every 20 seconds

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void loop() {
  if (client.connect(server, 80)) {
    
    // Measure Signal Strength (RSSI) of Wi-Fi connection
    int rssi = WiFi.RSSI();

    // Construct API request body
    String body = "&field1=";
      body += String(rssi);
      body += "&field2='" + ssid + "'";
    
    Serial.print("RSSI: ");
    Serial.println(rssi); 

    // Format: https://api.thingspeak.com/update?api_key=[Key]&field1=123&field2='value2' 
    client.print("GET https://api.thingspeak.com/update?api_key=");
    client.print(writeAPIKey);
    client.print(body);
    client.println(""); 
  }
  client.stop();

  // wait and then post again
  delay(postingInterval);
}

