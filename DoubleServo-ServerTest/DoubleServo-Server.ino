#include "Arduino_LED_Matrix.h"
ArduinoLEDMatrix matrix;

#include <Servo.h>
Servo myservo;
Servo myservo2;

#include "WiFiS3.h"

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

//Variables partie réseau
char ssid[] = "Freebox-C826F1_5GHz";        // SSID
char pass[] = "3xbz4qhnvcqwwdmvqnnzr5";    // Mot de passe du réseau
int keyIndex = 0;                 // Index utile pour WEP
byte mac[6];        // variable de 6 octets de long pour venir stocker l'adresse MAC de la carte
int status = WL_IDLE_STATUS;

// La communication avec le serveur se fera sur le port 80
WiFiServer server(80);

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

// Setup partie Réseau ---------------------------------------------------------------------------------------------

  WiFi.macAddress(mac); //Stockage de l'adresse MAC de la carte

  // Attente du module Wi-fi
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  // Vérification de la version du firmware
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // En attente de connection au réseau
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    
    // Connection au réseau
    status = WiFi.begin(ssid, pass);

    // Essai effectué toutes les 10 secondes
    delay(10000);
  }

  // Lancement du serveur une fois que la connection au réseau est établie
  server.begin();

  // Impression des infos de connection

  // Impression du SSID
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Impression de l'adresse IP
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // Impression de la force du signal
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  // Impression de l'adresse MAC
  Serial.print("Adresse MAC : ");
  Serial.print(mac[0],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.println(mac[5],HEX);
}


void loop() {

// PARTIE OPERATIVE ---------------------------------------------------------------------------------------------

    //Gestion Mode
    if (digitalRead(9)==0){
        move++;
    }

    if (move == 2){
      move = 0;
    }

    //Mode MANU ---

    //Calcul des positions des servos
    //myservo.write(180-analogRead(x)*180/1023); 
    //myservo2.write(180-analogRead(y)*180/1023); 

    //Mode AUTO ---
    
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
      //incrémentation jusqu'à 0°
      check = 0;
      cmp = cmp - 1;
      myservo.write(cmp);

    };

      //Emission d'un signal de 10µs
      digitalWrite(trigg, true);
      delayMicroseconds(10);
      digitalWrite(trigg, false);

      //Mesure du temps (en microsecondes) qui sépare l'émission et la réception du signal de 10µs émis 
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


// PARTIE RESEAU ---------------------------------------------------------------------------------------------

WiFiClient client = server.available();

  //  Vérifie si un client se connecte
  if (client) {

    Serial.println("new client");
    // an HTTP request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {

      if (client.available()) {


        // Données envoyées par le serveur au client
        char c = client.read();
        Serial.write(c);

        // Si vous êtes arrivé à la fin de la ligne (si vous avez reçu un caractère de nouvelle ligne)
        // et que la ligne est vide, cela signifie que la requête HTTP est terminée,
        // donc vous pouvez envoyer une réponse.

        if (c == '\n' && currentLineIsBlank) {
          
          // Envoie d'une réponse HTTP du serveur
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          
          client.print("X : ");
          client.println(analogRead(x));
          client.print("Y : ");
          client.println(analogRead(y));
          client.print("Angle : ");
          client.println(cmp);
          client.print("Distance : ");
          client.print(distance);

          client.println("</html>");

          Serial.println("!REPONSE ENVOYEE!");
          break;
        }
        if (c == '\n') {

          currentLineIsBlank = true;

        } else if (c != '\r') {

          currentLineIsBlank = false;

        }
      }
    }
    
    // Tempo pour laisser le temps au serveur de recevoir les données
    delay(10);

    // Fermeture de la connexion du client au serveur
    client.stop();
    Serial.println("client disconnected");
  }

  delay(10);

}


