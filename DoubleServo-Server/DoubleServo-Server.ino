#include "WiFiS3.h"

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
          client.println("Connection: close");  // Fermeture de la connexion après chaque envoie de réponse
          client.println("Refresh: 5");  // la page se rafraichit toutes les ... secondes
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          
          client.print("X : ");
          client.println("VALUE");
          client.print("Y : ");
          client.println("VALUE");
          client.print("Angle : ");
          client.println("VALUE");
          client.print("Distance : ");
          client.print("VALUE");

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


