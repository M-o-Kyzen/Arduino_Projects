#include <Servo.h>
Servo myservo;

int x = A1;
int y = A0;
int ms = 7;


float vectX;
float vectY;
float vectRefX = 518;
float vectRefY = 0;
float angle;


void setup() {

  Serial.begin(9600);

  //définition de la sortie PWM pour piloter le servomoteur
  myservo.attach(3);
  //définition des entrées du joystick
  pinMode(x, INPUT);
  pinMode(y, INPUT);
  pinMode(ms, INPUT);

}

void loop() {
  //définition des composantes du vecteur crée par le joystick
  vectX = analogRead(x) - 499;
  vectY = analogRead(y) - 525;
  //définition des composantes du vecteur référence
  vectRefX = 0 - 499;
  vectRefY = 518 - 525;

  //Sécurité pour prévoir la dérive du joystick en position repos
  if ( 490 > analogRead(x) || analogRead(x) > 505  && 520 > analogRead(x) || analogRead(x) > 530 ){
    //Calcul de l'angle entre le vecteur crée avec le déplacement du joystick et le vecteur de référence en y = 0
    angle = (180*acos((vectX * vectRefX + vectY * vectRefY) / (sqrt((pow(vectX, 2)) +  (pow(vectY, 2)) ) * sqrt(pow(vectRefX, 2) + pow(vectRefY, 2)))))/3,14;

  };

  //log
  Serial.print("x : ");
  Serial.println(analogRead(x));
  Serial.print("y : ");
  Serial.println(analogRead(y));
  Serial.print("vectx : ");
  Serial.println(vectX);
  Serial.print("vecty : ");
  Serial.println(vectY);
  Serial.print("angle : ");
  Serial.println(angle);
  Serial.println(" ");
  myservo.write(180-angle);
  


  delay(10);

}

