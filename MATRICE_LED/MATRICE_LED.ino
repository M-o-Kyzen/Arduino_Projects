#include "Arduino_LED_Matrix.h"
#include "animation.h" 
#include "frames.h" 

ArduinoLEDMatrix matrix;


void setup() {
matrix.begin();
Serial.begin(115200);

matrix.loadSequence(animation);
matrix.play(true);

}

void loop() {
delay(500);
Serial.println(millis());
}

/*
byte frame[8][12] = {
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 },
  { 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1 },
  { 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1 },
  { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }

};
matrix.renderBitmap(frame, 8, 12);
*/