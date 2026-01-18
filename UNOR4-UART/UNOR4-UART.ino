//#include <SoftwareSerial.h>
//SoftwareSerial test(10, 11); //RX et TX



void setup() {

/*
  test.begin(9600);
  Serial.begin(115200);
*/
  Serial1.begin(9600);
  Serial.begin(115200);
}
void loop() {
  
/*
  test.write("Hello Boss");
  Serial.println("Hello Boss");
*/

  Serial.print(Serial1.readString());

  delay(1000);
}