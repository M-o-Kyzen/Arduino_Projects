#include <Arduino.h>
#include <Wire.h>
#include <I2CKeyPad.h>
#include <LiquidCrystal.h>
#include <PCF8574.h>

//Mise en place du bus I2C pour le scanner
#define Scanner Wire
byte error, address;
int nDevices;

//Instanciation de l'écran lcd, ici câblé dans le but d'avoir une com basé sur 4 bits.
const int rs = 13, en = 12, d4 = 3, d5 = 4, d6 = 5, d7 = 6;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Adressage du premier PCF8574, utilisé pour le pad
I2CKeyPad keyPad(0x20);
uint32_t start, stop;
uint32_t lastKeyPressed = 0;
uint8_t index;

//Définitions des symboles qui correspondront aux touches du pad
char keys[] = "123A456B789C*0#D N";  //  N = NoKey, F = Fail
int menu_enabled;

//Adressage du deuxième PCF8574, utilisé pour les 2 capteurs à ultrasons
//PCF8574 UltrasoundSensors(0x21);

byte I2C_adresses[127];

//Classe pour obtenir les données des capteurs à ultrasons via un module PCF8574
class Ultrasound_Overall{
  
  //accessible seulement dans la classe
  private:
    //instanciation
    PCF8574 UltrasoundSensors;

    float distance;
    float time;
    int trigg;
    int echo;

  //accessible après instanciation par le biais des objets
  public:

    //On utilise le constructeur pour paramétré les classes et méthodes
    Ultrasound_Overall(int pin1,int pin2) : UltrasoundSensors(0x21){
      
      UltrasoundSensors.pinMode(pin1, OUTPUT);
      UltrasoundSensors.pinMode(pin2, INPUT);
      UltrasoundSensors.digitalWrite(pin1, LOW);
      trigg = pin1;
      echo = pin2;

    }

    void init(){

      Serial.println("Instance Ultrasound_Overall créee");
      Serial.println();

      if (!UltrasoundSensors.begin()) {
        Serial.println(F("ERROR: Could not initialize PCF8574! Check wiring, I2C address, SDA/SCL connections and power."));
        while (1) delay(100);
      }
      UltrasoundSensors.begin();
      Serial.println(F("PCF8574 initialized successfully\n"));
   
     }
    
    float mesure(){
      UltrasoundSensors.digitalWrite(trigg, LOW);
      UltrasoundSensors.digitalWrite(trigg, HIGH);
      delayMicroseconds(10);
      UltrasoundSensors.digitalWrite(trigg, LOW);

      //Mesure du temps (en microsecondes) qui sépare l'émission et la réception du signal de 10µs émis 
      time = UltrasoundSensors.pulseIn(echo, HIGH);

      //Calcul de la distance avant le rebond du signal (d=v*t) en centimètre
      distance = ((time / 1000000) / 2) * 340 * 100; // 1 s = 1 000 000 µs | On divise par 2 le temps pour obtenir la durée avant rebond

      return distance;
    }
    
};
//Instanciation des 2 capteurs. NOTE : Il serait plus judicieux d'avoir une seule config pour les 2 instance. Ici on vient config le PCFF8574 2 fois de la même façon, on perd en clareté.
Ultrasound_Overall capteur_ultrason1(7,6);
Ultrasound_Overall capteur_ultrason2(5,4);

int menu_selected = 0;
int volume_contenant;
int phase_A = 1;
int phase_B = 1;
int phase_C = 1;
String digit_buffer[10];
String digit_buffer2[10];
String digit_concat;
int rapport_diluant;
int rapport_alcool;
float distance;
float time;
float distance2;
float time2;
float volume_cuve1;
float volume_cuve2;
float volume_contenant_cuve1;
float volume_contenant_cuve2;
int start_timer_EV1;
int delta_time_EV1;
int start_timer_EV2;
int delta_time_EV2;
int pilotage_EV1;
int pilotage_EV2;
int start_timer_U1;
int delta_time_U1;
int start_timer_U2;
int delta_time_U2;
bool one_time[50];
int cpt_animation = 0;

void setup()
{
  //Définition du nombre de colonnes/lignes de l'écran
  lcd.begin(16, 2);
  
  Serial.begin(115200);
  Serial.println();
  Serial.println(__FILE__);
  Serial.print("I2C_KEYPAD_LIB_VERSION: ");
  Serial.println(I2C_KEYPAD_LIB_VERSION);
  Serial.println();

  //Pilotages des EV
  //Cuve Gauche (Diluant)
  //Fermeture
  pinMode(8, OUTPUT);
  //Ouverture
  pinMode(9, OUTPUT);

  //Cuve Droite (Alcool)
  //Fermeture
  pinMode(10, OUTPUT);
  //Ouverture
  pinMode(11, OUTPUT);

// Initialisation des PCF8574
  capteur_ultrason1.init();
  capteur_ultrason2.init();

  Wire.begin();
  Wire.setClock(400000);
  if (keyPad.begin() == false)
  {
    Serial.println("\nERROR: cannot communicate to keypad.\nPlease reboot.\n");
    while(1);
  }

  keyPad.setKeyPadMode(I2C_KEYPAD_4x4);
  menu_enabled = 1;

  //lancement du scanner
  Scanner.begin();

  //Rapports de dilution par défaut
  digit_buffer2[0] = 5;
  digit_buffer2[1] = 1;

  //Initiation des vannes
  digitalWrite(8, true);
  digitalWrite(10, true);
  lcd.print("Initialisation..");
  delay(9000);
  lcd.clear();
  digitalWrite(8, false);
  digitalWrite(9, false);
  digitalWrite(10, false);
  digitalWrite(11, false);
  
  lcd.noCursor();
}


void loop()
{
/*
Menus :

A : Cycle Auto
      ∟ Volume
        ∟ Rapport de dilution
          ∟ Lancer la production ou annuler
            ∟ Production en cours
B : Cycle Manuel
      ∟ EV1 et valeurs volumes
        ∟ EV2 et valeurs capteurs
C : IHM
      ∟ Valeurs capteurs 
        ∟ état des bus PCF8574
*/

  //obtention de l'index de la touche pressée
  index = keyPad.getKey();

  //Inilisation des booléens         
  for (int i = 0; i < 50; i++) {
    one_time[i] = true;
  };

  lcd.clear();

  //Gestion des menus
  if (menu_enabled == 1){
    switch (index)
    {
        case 3 /*A : Menu A*/ :
          menu_enabled = 0;
          Serial.print("Menu A");
          menu_selected = 1;
        break;
        case 7 /*B : Menu B*/ :
          menu_enabled = 0;
          Serial.print("Menu B");
          menu_selected = 2;
        break;
        case 11 /*C : Menu C*/ :
          menu_enabled = 0;
          Serial.print("Menu C");
          menu_selected = 3;
        break;
        case 15 /*D : Menu D*/ :
          menu_enabled = 0;
          Serial.print("Menu D");
          menu_selected = 4;
        break;
    }
  }

  //Contenu des menus
  switch (menu_selected)
    {
        case 0 /*Menu Principal*/ :
          index = keyPad.getKey();
          lcd.setCursor(0, 0); 
          lcd.print("MACHINE A RIFLON");
 
          cpt_animation = cpt_animation + 1;

          lcd.setCursor(cpt_animation-1, 1);
          lcd.print(" ");
          lcd.setCursor(cpt_animation, 1);
          lcd.print(">");
          //lcd.display();
          delay(300);

          if (cpt_animation == 16){
            cpt_animation = 0;
          }

          
        break;
        case 1 /*Menu A*/ :
        //Renseignement du volume du verre
          while (phase_A == 1){
            index = keyPad.getKey();
            lcd.setCursor(0, 0); 
            lcd.print("Volume : ");
            if (index >= 0 && index < 3 || index >= 4 && index < 7 || index >= 8 && index < 11 || index == 13){
              
              //Récupération de la valeur rentrée
              digit_buffer[2] = digit_buffer[1];
              digit_buffer[1] = digit_buffer[0];
              digit_buffer[0] = keys[index];
              digit_concat = digit_buffer[0] + digit_buffer[1] + digit_buffer[2];
              Serial.println(digit_concat);
              /*
              //Info buffer
              for (int i = 0; i < 3; i++) {
                Serial.print("[");
                Serial.print(digit_buffer[i]);
                Serial.print("] ");
              };
            
              Serial.println();
              */
            }
            lcd.print(digit_concat);
            lcd.print(" mL");
            //lcd.display();
            //lcd.clear();
            delay(300);

            //Touche entrée
            if (index == 14){
              phase_A = 2;
              lcd.clear();
              volume_contenant = digit_concat.toInt();
              Serial.print("Volume : ");
              Serial.print(volume_contenant);
              break;
            }
          }

          //Renseignement du rapport de dilution
          while (phase_A == 2){
            index = keyPad.getKey();
            lcd.setCursor(4, 0); 
            lcd.print("Dilution");
            if (index >= 0 && index < 3 || index >= 4 && index < 7 || index >= 8 && index < 11 || index == 13){
              
              //Récupération de la valeur rentrée
              digit_buffer2[1] = digit_buffer2[0];
              digit_buffer2[0] = keys[index];
              Serial.print(digit_buffer2[0]);
              Serial.print(" <> ");
              Serial.println(digit_buffer2[1]);
            }

            lcd.setCursor(0, 1);
            lcd.print("Dilu "); 
            lcd.print(digit_buffer2[0]);
            lcd.print(" <> ");
            lcd.print(digit_buffer2[1]);
            lcd.print(" Alcl"); 
            //lcd.display();
            //lcd.clear();
            delay(300);

            //Touche entrée
            if (index == 14){
              phase_A = 3;
              lcd.clear();
              rapport_diluant = digit_buffer2[0].toInt();
              rapport_alcool = digit_buffer2[1].toInt();
              Serial.print("Rapport de dilution Diluant/Alcool : ");
              Serial.print(rapport_diluant);
              Serial.print(" <> ");
              Serial.println(rapport_alcool);
              break;
            }          
          
          }

          //Validation du cycle
          while (phase_A == 3){
            index = keyPad.getKey();
            lcd.setCursor(0, 0); 
            lcd.print(volume_contenant);
            lcd.print(" | ");
            lcd.print(rapport_diluant);
            lcd.print(" <> ");
            lcd.print(rapport_alcool);
            lcd.setCursor(0, 1);
            lcd.print("Valider 1:Y 2:N");
            //lcd.display();
            //lcd.clear();
            delay(300);

            if (index == 0){
              Serial.print("Production lancée");
              phase_A = 4;
              lcd.clear();
              break;
              //Sortie du menu A
            } else if (index == 1){
              Serial.print("Production annulée");
              menu_enabled = 1;
              phase_A = 1;
              phase_B = 1;
              phase_C = 1;
              menu_selected = 0;
              lcd.clear();
              break;
            }       
          
          }
          
          //Production en cours
          while (phase_A == 4){

            //Mesure des hauteurs
            distance = capteur_ultrason1.mesure();
            distance2 = capteur_ultrason2.mesure();

            volume_cuve1 = (distance*0.01 * 0.003025 * 3.14)*1000; //Calcul du volume de la cuve 1 en litre
            volume_cuve2 = (distance2*0.01 * 0.003025 * 3.14)*1000; //Calcul du volume de la cuve 2 en litre
            
            volume_contenant_cuve1 = rapport_diluant / (rapport_diluant + rapport_alcool) * volume_contenant;
            volume_contenant_cuve2 = rapport_alcool / (rapport_diluant + rapport_alcool) * volume_contenant;

            //Initialisation du temps de pilotage pour ouvrir/fermer les EV
            if (one_time[0]){
              start_timer_EV1 = millis();
              start_timer_EV2 = millis();
              one_time[0] = false;
            }

            if (volume_contenant_cuve1 < volume_cuve1){
              pilotage_EV1 = 1;
            } else {
              if (one_time[1]){
                start_timer_EV1 = millis();
                delta_time_EV1 = 0;
              }
              one_time[1] = false;

              pilotage_EV1 = 0;
            }

            if (volume_contenant_cuve2 < volume_cuve2){
              pilotage_EV2 = 1;
            } else {
              if (one_time[2]){
                start_timer_EV2 = millis();
                delta_time_EV2 = 0;
              }
              one_time[2] = false;
              
              pilotage_EV2 = 0;
            }

            delta_time_EV1 = millis() - start_timer_EV1;
            //Ouverture EV1
            if (delta_time_EV1 <= 9000 && pilotage_EV1 == 1){
              digitalWrite(9, true);
            } else {
              digitalWrite(9, false);
            }

            //Fermeture EV1
            if (delta_time_EV1 <= 9000 && pilotage_EV1 == 0){
              digitalWrite(8, true);
            } else {
              digitalWrite(8, false);
            }

            delta_time_EV2 = millis() - start_timer_EV2;
            //Ouverture EV2
            if (delta_time_EV2 <= 9000 && pilotage_EV2 == 1){
              digitalWrite(11, true);
            } else {
              digitalWrite(11, false);
            }

            //Fermeture EV2
            if (delta_time_EV2 <= 9000 && pilotage_EV2 == 0){
              digitalWrite(10, true);
            } else {
              digitalWrite(10, false);
            }   
          
          }
        break;
        case 2 /*Menu B*/ :
          //Pilotage  EV1
          while (phase_B == 1){
            index = keyPad.getKey();

            //init du timer de calcul du volume
            if (one_time[5]){
              start_timer_U1 = millis();
              one_time[5] = false;
            }

            //Mesure des hauteurs
            distance = capteur_ultrason1.mesure();
            distance2 = capteur_ultrason2.mesure();

            delta_time_U1 = millis() - start_timer_U1;

            if (delta_time_U1 <= 1000){
              if (one_time[3]){

                volume_cuve1 = (distance*0.01 * 0.003025 * 3.14)*1000; //Calcul du volume de la cuve 1 en litres

                one_time[3] = false;
              }
            } else {
              start_timer_U1 = millis();
              one_time[3] = true;
            }
            
            if (index == 0){
              digitalWrite(9, true);
            } else {
              digitalWrite(9, false);
            }

            if (index == 1){
              digitalWrite(8, true);
            } else {
              digitalWrite(8, false);
            }

            lcd.setCursor(0, 0); 
            lcd.print("EV1 1->O 2->F");
            lcd.setCursor(0, 1); 
            lcd.print("Volume : ");
            lcd.print(volume_cuve1);
            lcd.print(" L");

            //Passage à EV2
            if (index == 14){
              Serial.print("Pilotage EV2");
              phase_B = 2;
              lcd.clear();
              break;
            } 
          }
          while (phase_B == 2){
            index = keyPad.getKey();

            //init du timer de calcul du volume
            if (one_time[6]){
              start_timer_U2 = millis();
              one_time[6] = false;
            }

            //Mesure des hauteurs
            distance = capteur_ultrason1.mesure();
            distance2 = capteur_ultrason2.mesure();
            delta_time_U2 = millis() - start_timer_U2;

            if (delta_time_U2 <= 1000){
              if (one_time[4]){

                volume_cuve2 = (distance2*0.01 * 0.003025 * 3.14)*1000; //Calcul du volume de la cuve 2 en litres

                one_time[4] = false;
              }
            } else {
              start_timer_U2 = millis();
              one_time[4] = true;
            }
            if (index == 0){
              digitalWrite(11, true);
            } else {
              digitalWrite(11, false);
            }

            if (index == 1){
              digitalWrite(10, true);
            } else {
              digitalWrite(10, false);
            }

            lcd.setCursor(0, 0); 
            lcd.print("EV2 1->O 2->F");
            lcd.setCursor(0, 1); 
            lcd.print("Volume : ");
            lcd.print(volume_cuve2);
            lcd.print(" L");

            //Sortie du menu B
            if (index == 12 /*'*' : Retour*/){
              Serial.print("Retour");
              menu_enabled = 1;
              phase_A = 1;
              phase_B = 1;
              phase_C = 1;
              menu_selected = 0;
              lcd.clear();
              delay(300);
              break;
            }
          }
 
        break;
        case 3 /*Menu C*/ :
          //Valeurs capteurs
          while (phase_C == 1){
            index = keyPad.getKey();

            //Mesure des hauteurs
            distance = capteur_ultrason1.mesure();
            distance2 = capteur_ultrason2.mesure();

            lcd.setCursor(0, 0); 
            lcd.print("Distance : ");
            lcd.print(distance);
            lcd.print(" cm");

            lcd.setCursor(0, 1); 
            lcd.print("Distance : ");
            lcd.print(distance2);
            lcd.print(" cm");
            
            delay(500);

            //Passage aux bus PCF8574
            if (index == 14){
              Serial.print("Bus PCF8574");
              phase_C = 2;
              lcd.clear();
              break;
            } 
          }

          while (phase_C == 2){
            index = keyPad.getKey();

            //clear du tableau des adresses I2C
            for(int i = 0; i < 127; i++ ){
              I2C_adresses[i] = -1;
            }

            nDevices = 0;
            for(address = 1; address < 127; address++ )
            {
              // The i2c_scanner uses the return value of
              // the Write.endTransmisstion to see if
              // a device did acknowledge to the address.
              Scanner.beginTransmission(address);
              error = Scanner.endTransmission();

              if (error == 0)
              {
                Serial.print("I2C device found at address 0x");
                if (address<16){
                  Serial.print("1000");
                }               
                Serial.print(address,HEX);
                Serial.println("  !");

                I2C_adresses[address] = address;

                nDevices++;
              }
              else if (error==4)
              {
                Serial.print("Unknown error at address 0x");
                if (address<16){
                  Serial.print("0");
                }
                Serial.println(address,HEX);
              }
            }

            lcd.setCursor(0, 0); 
            lcd.print("Appareils I2C :");
            lcd.setCursor(0, 1);

            for(int i = 0; i < 127; i++ ){
              if (I2C_adresses[i] != 0xFF){
                lcd.print(" x");
                lcd.print(I2C_adresses[i],HEX);
              }
            }
            lcd.setCursor(0, 0);

            if (nDevices == 0){
              Serial.println("No I2C devices found\n");
            } else{
              Serial.println("done\n");
            }

            delay(1000);

            //Sortie du menu C
              if (index == 12 /*'*' : Retour*/){
              Serial.print("Retour");
              menu_enabled = 1;
              phase_A = 1;
              phase_B = 1;
              phase_C = 1;
              menu_selected = 0;
              lcd.clear();
              delay(300);
              break;
              }
          }
        break;
        case 4 /*Menu D*/ :

        break;
    }
}