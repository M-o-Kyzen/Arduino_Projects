/*
 * Name: arduino_uno_interrupt_button_read
 *
 * Author: Renzo Mischianti
 * Website: https://www.mischianti.org
 *
 * This example demonstrates how to use an interrupt to read a button press on an Arduino Uno.
 * The PCF8574's INT pin is connected to an interrupt-capable pin on the Arduino.
 *
 * Connections:
 * PCF8574    ----- Arduino Uno
 * A0, A1, A2 ----- VCC (to set I2C address to 0x27) or GND (to set I2C address to 0x20)
 *                In this example the address is 0x39 (PCF8574A)
 * VSS        ----- GND
 * VDD        ----- 5V
 * SDA        ----- A4
 * SCL        ----- A5
 * INT        ----- D2 (interrupt pin)
 *
 * P1         ----- Connect to a button.
 * P0         ----- Unconnected (set as output).
 *
 */

#include "Arduino.h"
#include <LiquidCrystal.h>
#include "PCF8574.h"  // https://github.com/xreef/PCF8574_library

// Define the interrupt pin on the Arduino Uno (only pins 2 and 3 are interrupt-capable)
#define ARDUINO_UNO_INTERRUPTED_PIN 2

// Forward declaration of the interrupt service routine
void keyPressedOnPCF8574();

// Initialize the PCF8574 library with the I2C address, interrupt pin, and ISR
PCF8574 pcf8574(0x20, ARDUINO_UNO_INTERRUPTED_PIN,keyPressedOnPCF8574);

// Volatile flag to be set by the ISR
bool keyPressed = false;

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 13, en = 12, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600);
  if (pcf8574.begin()){
    Serial.println("OK");
  } else {
    Serial.println("KO");
  }
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
}

void loop() {
  // Turn off the display:
  lcd.noDisplay();
  delay(500);
  // Turn on the display:
  lcd.display();
  delay(500);
}

void keyPressedOnPCF8574(){
  // Set the flag to indicate that a key has been pressed
  keyPressed = true;
}