#include "Arduino_LED_Matrix.h"
ArduinoLEDMatrix matrix;

//Connection à la base de donnée et envoie de la requête
#include <Ethernet.h>
/*
#include <MySQL_Connection.h>
byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server_addr(192,168,0,27);  // IP du serveur SQL
char user[] = "matteo";              // Nom d'utilisateur du compte
char password[] = "patate";        // Mot de passe du compte
EthernetClient client;
MySQL_Connection conn((Client *)&client);
char INSERT_SQL[] = "Insert Liste VALUES(33,2.2)";
*/

int trigg = 13;
int echo = 12;
float distance;
int col0; int col1; int col2; int col3; int col4; int col5; int col6; int col7; int col8; int col9; int col10; int col11;



void setup() {
  
    Serial.begin(9600);
    //Définition des E/S
    pinMode(trigg, OUTPUT);
    pinMode(echo, INPUT);

    //reset du signal au cas où
    digitalWrite(trigg, false);

    matrix.begin();
    matrix.play(true);

    // Fonction d'envoie de requête à la base SQL
    //SQL_Connection();
    
}

void loop() {

    //Emission d'un signal de 10µs
    digitalWrite(trigg, true);
    delayMicroseconds(10);
    digitalWrite(trigg, false);

    //Mesure du temps (en microsecondes) qui sépare l'émission et la réception du signal de 10µs émis 
    float time = pulseIn(echo, true);

    //Calcul de la distance avant le rebond du signal (d=v*t) en centimètre
    float distance = ((time / 1000000) / 2) * 340 * 100; // 1 s = 1 000 000 µs | On divise par 2 le temps pour obtenir la durée avant rebond

    Serial.print("Distance : ");
    Serial.print(distance); //en cm
    Serial.println(" cm");
    Serial.print("temps : ");
    Serial.print(time);
    Serial.println(" µs");
    Serial.println("");


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
    
    delay(500);

};

/*

//Connexion au serveur SQL
void SQL_Connection() {

  while (!Serial); // wait for serial port to connect
  Ethernet.begin(mac_addr);
  Serial.println("Connecting...");
  if (conn.connect(server_addr, 3306, user, password)) {
    delay(1000);
  }
  else
    Serial.println("Connection failed.");
  conn.close();

  };

*/
