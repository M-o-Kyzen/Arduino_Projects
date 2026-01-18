#include <SPI.h>
#include <WiFi.h>

char ssid[] = "Freebox-C826F1_5GHz";          //  your network SSID (name)
char pass[] = "3xbz4qhnvcqwwdmvqnnzr5";   // your network password
int status = WL_IDLE_STATUS;
byte mac[6];

WiFiServer server(80);


void setup() {
  // initialize serial:
  Serial.begin(9600);
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);


  status = WiFi.begin(ssid, pass);
  WiFi.macAddress(mac);

  if ( status != WL_CONNECTED) {

    Serial.println("Couldn't get a wifi connection");
    while(true);

  } else {

    server.begin();
    IPAddress myAddress = WiFi.localIP();
    Serial.print("Connected to wifi. My address : ");
    Serial.println(myAddress);
    Serial.print("Mac adress : ");
    Serial.print(mac[0],HEX);
    Serial.print(":");
    Serial.print(mac[1],HEX);
    Serial.print(":");
    Serial.print(mac[2],HEX);
    Serial.print(":");
    Serial.print(mac[3],HEX);
    Serial.print(":");
    Serial.print(mac[4],HEX);
    Serial.print(":");
    Serial.println(mac[5],HEX);

  }
}


void loop() {
    // listen for incoming clients
 
}