#include "Arduino_LED_Matrix.h"
ArduinoLEDMatrix matrix;

#include <Servo.h>
Servo myservo;
Servo myservo2;


//Variables partie opérative
int x = A1;
int y = A0;
int ms = 7;

int trigg = 13;
int echo = 12;
float distance;
int col0; int col1; int col2; int col3; int col4; int col5; int col6; int col7; int col8; int col9; int col10; int col11;

int move;

int cmp;
int check;

bool Manu;
bool upfront;


void setup() {

  Serial.begin(250000);
  while (!Serial) {
    ; 
  }
  
// Setup partie Opérative ---------------------------------------------------------------------------------------------

  //définition des sorties PWM pour piloter les servomoteurs
  myservo.attach(3);
  myservo2.attach(5);
  
  //définition des entrées du joystick
  pinMode(x, INPUT);
  pinMode(y, INPUT);
  pinMode(ms, INPUT);

  //définition du bouton
  pinMode(9, INPUT);

  //Définition des E/S
  pinMode(trigg, OUTPUT);
  pinMode(echo, INPUT);

  //reset du signal au lancement du programme au cas où
  digitalWrite(trigg, false);

  matrix.begin();
  matrix.play(true);

  //Conditions initiales pour le cycle de rotation
  cmp = 40;
  check = 1;
  Manu = false;
  
}


void loop() {

// PARTIE OPERATIVE ---------------------------------------------------------------------------------------------

    //Gestion Mode
    //Incrémentation du compteur de gestion de mode
    if (digitalRead(9)==0){
        move++;
    }

    //Passage en mode manu (tempo d'une seconde pour contrer la vitesse d'execution du programme)
    if (move == 1) {

      while (!upfront){

        byte matrice[8][12] = {
          {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
          {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
          {1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
          {1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1},
          {1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1},
          {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1},
          {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
          {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
        };

        matrix.renderBitmap(matrice, 8, 12);

        delay(1000);
        upfront = true;

      }

      Manu = true;
      
    }

    //Passage en mode auto (tempo d'une seconde pour contrer la vitesse d'execution du programme)
    if (move == 2){

      while (upfront){

        byte matrice[8][12] = {
          {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
          {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
          {0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
          {0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0},
          {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
          {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
          {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
          {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
        };

        matrix.renderBitmap(matrice, 8, 12);

        delay(2000);
        upfront = false;

      }

      Manu = false;
      move = 0;

      //Conditions initiales
      digitalWrite(trigg, false);
      cmp = 40;
      check = 1;

    }

  //Mode MANU ---
  if (Manu){

    //Calcul des positions des servos
    myservo.write(180-analogRead(x)*180/1023); 
    myservo2.write(180-analogRead(y)*180/1023);

  };

    //Mode AUTO ---
  
  if (!Manu){

    // remise à 0 du cyle
    if (cmp == 40){
      check = 1;
    };

    //condition pour commencer le cycle
    if (cmp < 140 && check == 1 ) {
      //incrémentation jusqu'à 180°
      cmp = cmp + 1;
      myservo.write(cmp);

    } else {
      //décrémentation jusqu'à 0°
      check = 0;
      cmp = cmp - 1;
      myservo.write(cmp);

    };

      //Emission d'un signal de 10µs
      digitalWrite(trigg, true);
      delayMicroseconds(10);
      digitalWrite(trigg, false);

      //Mesure du temps (en microsecondes) qui sépare l'émission et la réception du signal de 10µs émis, avec un Time Out de 15 ms pour éviter les ralentissements
      float time = pulseIn(echo, true, 15000);

      //Calcul de la distance avant le rebond du signal (d=v*t) en centimètre
      float distance = ((time / 1000000) / 2) * 340 * 100; // 1 s = 1 000 000 µs | On divise par 2 le temps pour obtenir la durée avant rebond

      //log

      Serial.print("Distance : ");
      Serial.print(distance); //en cm
      Serial.println(" cm");
      Serial.print("temps : ");
      Serial.print(time);
      Serial.println(" μs");
      Serial.println("");
      Serial.print("x : ");
      Serial.println(analogRead(x));
      Serial.print("y : ");
      Serial.println(analogRead(y));
      Serial.print("angle : ");
      Serial.println(cmp);
      Serial.print("mode : ");
      Serial.println(move);
      Serial.println("---------------");

      int col[13]{ col0, col1, col2, col3, col4, col4, col5, col6, col7, col8, col9, col10, col11 };

      //Incrémentation de la matrice
      for (int i = 11; i >= (distance-10)/10; i--) {
          col[i] = 1;
      };

      byte matrice[8][12] = {
        {col[11], col[10], col[9], col[8], col[7], col[6], col[5], col[4], col[3], col[2], col[1], col[0]},
        {col[11], col[10], col[9], col[8], col[7], col[6], col[5], col[4], col[3], col[2], col[1], col[0]},
        {col[11], col[10], col[9], col[8], col[7], col[6], col[5], col[4], col[3], col[2], col[1], col[0]},
        {col[11], col[10], col[9], col[8], col[7], col[6], col[5], col[4], col[3], col[2], col[1], col[0]},
        {col[11], col[10], col[9], col[8], col[7], col[6], col[5], col[4], col[3], col[2], col[1], col[0]},
        {col[11], col[10], col[9], col[8], col[7], col[6], col[5], col[4], col[3], col[2], col[1], col[0]},
        {col[11], col[10], col[9], col[8], col[7], col[6], col[5], col[4], col[3], col[2], col[1], col[0]},
        {col[11], col[10], col[9], col[8], col[7], col[6], col[5], col[4], col[3], col[2], col[1], col[0]}
      };

      matrix.renderBitmap(matrice, 8, 12);

  delay(10);

  }

}


