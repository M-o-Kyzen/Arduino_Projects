#include <SoftwareSerial.h>
SoftwareSerial send(10, 11); //RX et TX



void setup() {


  send.begin(9600);
  Serial.begin(115200);

  //Serial1.begin(9600);
  Serial.begin(115200);
 

}
void loop() {
  
 send.println("A");


 // delay(1000);
}