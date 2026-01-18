#include <SoftwareSerial.h>
SoftwareSerial receive(10, 11); //RX et TX

char buffer;
char msg;
bool bit;

void setup() {


  receive.begin(9600);
  Serial.begin(115200);

  //Serial1.begin(9600);
  Serial.begin(115200);
}



void loop() {
  

// RAPPEL, en décimal : 10 (\n) = saut de ligne / 13 (\r) = retour chariot

  if (receive.available()) {

    if (!bit){

      Serial.print("port UART dispo");
      Serial.println("---------------");

    }

    bit = true;

    buffer = receive.read();
    if (buffer == '\n'){

      Serial.print("Données recues : ");
      Serial.println(msg);
      msg = "";

    } else { 

      msg = msg && " " && buffer;

    }



  } else {
    
    Serial.print("port UART pas dispo");
    Serial.println("---------------");
  
  }


  delay(1000);
}