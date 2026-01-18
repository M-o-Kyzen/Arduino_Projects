//#include "SoftwareSerial.h"
//SoftwareSerial test2(5, 6); //RX et TX




void setup() {

/*
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 5, 6);
//test2.begin(9600, SERIAL_8N1, 5, 6);
*/
  Serial.begin(115200);
  Serial2.begin(9600);

}
void loop() {

/*
    Serial.println("Message Received: ");
    Serial.println(Serial2.readString());
*/

  Serial2.write("2");


    delay(1000);
}

