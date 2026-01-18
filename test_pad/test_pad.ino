#include <Keypad.h> 
//four rows 
const byte ROWS = 4; 
//four columns 
const byte COLS = 4; 
//define the cymbols on the buttons of the keypads 
char hexaKeys[ROWS][COLS] = { 
{'1','2','3','A'}, 
{'4','5','6','B'}, 
{'7','8','9','C'}, 
{'*','0','#','D'} 
}; 



//connect to the row pinouts of the keypad 
byte rowPins[ROWS] = {16, 17, 18, 19}; 
//connect to the column pinouts of the keypad 
byte colPins[COLS] = {21, 22, 23, 25}; 
//initialize an instance of class NewKeypad 
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup(){ 
  Serial.begin(9600); 

} 



void loop(){ 

  char customKey = customKeypad.getKey(); 
  if (customKey){ 
    Serial.println(customKey);
    //delay(1000);
  }

}