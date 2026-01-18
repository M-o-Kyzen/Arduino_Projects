#include <Arduino.h>
#include <Wire.h>
#include <I2CKeyPad.h>
#include <LiquidCrystal.h>
#include <PCF8574.h>

const int rs = 13, en = 12, d4 = 3, d5 = 4, d6 = 5, d7 = 6;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

PCF8574 UltrasoundSensors(0x21);

I2CKeyPad keyPad(0x20);
uint32_t start, stop;
uint32_t lastKeyPressed = 0;


void setup()
{

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  //lcd.print("hello, world!");

  Serial.begin(115200);
  Serial.println();
  Serial.println(__FILE__);
  Serial.print("I2C_KEYPAD_LIB_VERSION: ");
  Serial.println(I2C_KEYPAD_LIB_VERSION);
  Serial.println();

  UltrasoundSensors.pinMode(P7, OUTPUT); // TRIG pin
  UltrasoundSensors.pinMode(P6, INPUT);  // ECHO pin
  UltrasoundSensors.digitalWrite(P7, LOW); // Initialize TRIG to LOW

  UltrasoundSensors.pinMode(P5, OUTPUT); // TRIG pin
  UltrasoundSensors.pinMode(P4, INPUT);  // ECHO pin
  UltrasoundSensors.digitalWrite(P5, LOW); // Initialize TRIG to LOW

  // Initialize PCF8574 after pins are configured
  if (!UltrasoundSensors.begin()) {
    Serial.println(F("ERROR: Could not initialize PCF8574! Check wiring, I2C address, SDA/SCL connections and power."));
    while (1) delay(100);
  }
  UltrasoundSensors.begin();
  Serial.println(F("PCF8574 initialized successfully\n"));

  Wire.begin();
  Wire.setClock(400000);
  if (keyPad.begin() == false)
  {
    Serial.println("\nERROR: cannot communicate to keypad.\nPlease reboot.\n");
    while(1);
  }

  keyPad.setKeyPadMode(I2C_KEYPAD_4x4);
}


void loop()
{

  UltrasoundSensors.digitalWrite(P7, HIGH);
  delayMicroseconds(10);
  UltrasoundSensors.digitalWrite(P7, LOW);

  //Mesure du temps (en microsecondes) qui sépare l'émission et la réception du signal de 10µs émis 
  float time = UltrasoundSensors.pulseIn(P6, HIGH);

  //Calcul de la distance avant le rebond du signal (d=v*t) en centimètre
  float distance = ((time / 1000000) / 2) * 340 * 100; // 1 s = 1 000 000 µs | On divise par 2 le temps pour obtenir la durée avant rebond

  UltrasoundSensors.digitalWrite(P5, HIGH);
  delayMicroseconds(10);
  UltrasoundSensors.digitalWrite(P5, LOW);

  //Mesure du temps (en microsecondes) qui sépare l'émission et la réception du signal de 10µs émis 
  float time2 = UltrasoundSensors.pulseIn(P4, HIGH);

  //Calcul de la distance avant le rebond du signal (d=v*t) en centimètre
  float distance2 = ((time2 / 1000000) / 2) * 340 * 100; // 1 s = 1 000 000 µs | On divise par 2 le temps pour obtenir la durée avant rebond
/*
  Serial.print("Distance : ");
  Serial.print(distance); //en cm
  Serial.println(" cm");
  Serial.print("temps : ");
  Serial.print(time);
  Serial.println(" µs");
  Serial.println("");
*/
  //  adjust keymap if needed
  char keys[] = "123A456B789C*0#D N";  //  N = NoKey, F = Fail

  uint8_t index = keyPad.getKey();

  if (index != 16)
  {
    Serial.print(keys[index]);
    lcd.print(keys[index]);
  }

  lcd.setCursor(0, 0); 
  lcd.print(distance);
  lcd.setCursor(0, 1);
  lcd.print(distance2);
  lcd.display();

  delay(200);
  lcd.clear();
}

