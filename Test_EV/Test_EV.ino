#include <Arduino.h>
#include <Wire.h>
#include <I2CKeyPad.h>
#include <LiquidCrystal.h>
#include <PCF8574.h>

const int rs = 13, en = 12, d4 = 3, d5 = 4, d6 = 5, d7 = 6;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Définition des E/S
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  digitalWrite(8, false);
  digitalWrite(9, false);

  lcd.begin(16, 2);
  lcd.print("TEST");
  lcd.display();
}

void loop() {
  digitalWrite(10, true);
  digitalWrite(8, true);
  delay(7000);
  digitalWrite(10, false);
  digitalWrite(8, false);

  digitalWrite(11, true);
  digitalWrite(9, true);
  delay(7000);
  digitalWrite(11, false);
  digitalWrite(9, false);
  delay(1000);
}
