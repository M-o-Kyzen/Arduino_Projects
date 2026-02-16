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
PCF8574 UltrasoundSensors(0x21);

byte I2C_adresses[127];

int menu_selected = 0;
float volume_contenant;
int phase_A = 1;
int phase_B = 1;
int phase_C = 1;
String digit_buffer[10];
String digit_buffer2[10];
String digit_concat;
float rapport_diluant;
float rapport_alcool;
float distance;
float time;
float distance2;
float time2;
float volume_cuve1_max = 2.58;
float volume_cuve2_max = 2.47;
float volume_cuve1;
float volume_cuve2;
float volume_cuve1_actuel;
float volume_cuve2_actuel;
float volume_contenant_cuve1;
float volume_contenant_cuve2;
float duree_EV1;
float duree_EV2;
float duree_tot_EV1;
float duree_tot_EV2;
int prod_ok;
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
bool one_time[15];
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

  //Définition des E/S des 2 capteurs à ultrasons
  UltrasoundSensors.pinMode(P7, OUTPUT);
  UltrasoundSensors.pinMode(P6, INPUT);
  UltrasoundSensors.digitalWrite(P7, LOW);

  UltrasoundSensors.pinMode(P5, OUTPUT);
  UltrasoundSensors.pinMode(P4, INPUT);
  UltrasoundSensors.digitalWrite(P5, LOW);

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
            //lcd.clear();
            delay(300);

            //Touche entrée
            if (index == 14){
              phase_A = 2;
              lcd.clear();
              volume_contenant = digit_concat.toInt();
              Serial.print("Volume : ");
              Serial.println(volume_contenant);
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
            lcd.print(volume_contenant,0);
            lcd.println(" | ");
            lcd.print(rapport_diluant,0);
            lcd.print(" <> ");
            lcd.print(rapport_alcool,0);
            lcd.setCursor(0, 1);
            lcd.print("Valider 1:Y 2:N");
            //lcd.display();
            //lcd.clear();
            delay(300);

            if (index == 0){
              Serial.println("Production lancée");
              phase_A = 4;
              lcd.clear();
              break;
              //Sortie du menu A
            } else if (index == 1){
              Serial.println("Production annulée");
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
            UltrasoundSensors.digitalWrite(P7, HIGH);
            delayMicroseconds(10);
            UltrasoundSensors.digitalWrite(P7, LOW);

            //Mesure du temps (en microsecondes) qui sépare l'émission et la réception du signal de 10µs émis 
            time = UltrasoundSensors.pulseIn(P6, HIGH);

            //Calcul de la distance avant le rebond du signal (d=v*t) en centimètre
            distance = ((time / 1000000) / 2) * 340 * 100; // 1 s = 1 000 000 µs | On divise par 2 le temps pour obtenir la durée avant rebond

            UltrasoundSensors.digitalWrite(P5, HIGH);
            delayMicroseconds(10);
            UltrasoundSensors.digitalWrite(P5, LOW);

            //Mesure du temps (en microsecondes) qui sépare l'émission et la réception du signal de 10µs émis 
            time2 = UltrasoundSensors.pulseIn(P4, HIGH);

            //Calcul de la distance avant le rebond du signal (d=v*t) en centimètre
            distance2 = ((time2 / 1000000) / 2) * 340 * 100; // 1 s = 1 000 000 µs | On divise par 2 le temps pour obtenir la durée avant rebond

            volume_cuve1_actuel = (distance*0.01 * 0.003025 * 3.14)*1000.0; //Calcul du volume de la cuve 1 en litre
            volume_cuve2_actuel = (distance2*0.01 * 0.003025 * 3.14)*1000.0; //Calcul du volume de la cuve 2 en litre
            

            //Obtention des volumes de départ des cuves ainsi que des temps d'ouverture des vannes
            if (one_time[7]){
              
              volume_contenant_cuve1 = (rapport_diluant / (rapport_diluant + rapport_alcool)) * (volume_contenant/1000);
              volume_contenant_cuve2 = (rapport_alcool / (rapport_diluant + rapport_alcool)) * (volume_contenant/1000);

              //volume_cuve1 = (distance*0.01 * 0.003025 * 3.14)*1000;
              //volume_cuve2 = (distance2*0.01 * 0.003025 * 3.14)*1000;
              //Shunt test
              volume_cuve1_actuel = 1.5;
              volume_cuve2_actuel = 1.5;

              //Calcul du temps d'ouverture des vannes eà partir du delta entre le volume actuel d'une cuve et le volume final voulu après production
              duree_EV1 = (volume_cuve1_actuel*9361.1-872.2) - ((volume_cuve1_actuel-volume_contenant_cuve1)*9361.1-872.2);
              duree_EV2 = (volume_cuve2_actuel*9361.1-872.2) - ((volume_cuve2_actuel-volume_contenant_cuve2)*9361.1-872.2);
              
              //On ajoute les temps d'ouvertures complets des EV
              duree_tot_EV1 = duree_EV1 + 9000;
              duree_tot_EV2 = duree_EV2 + 9000;

              Serial.print("Volume voulu dans cuve 1 : ");
              Serial.println(volume_contenant_cuve1, 5);
              Serial.print("Volume voulu dans cuve 2 : ");
              Serial.println(volume_contenant_cuve2, 5);
              Serial.print("Volume actuel cuve 1 : ");
              Serial.println(volume_cuve1_actuel, 5);
              Serial.print("Volume actuel cuve 2 : ");
              Serial.println(volume_cuve2_actuel, 5);
              Serial.print("Temps d'ouverture voulu pour EV1 : ");
              Serial.println(duree_EV1, 5);
              Serial.print("Temps d'ouverture voulu pour EV2 : ");
              Serial.println(duree_EV2, 5);
              
              one_time[7] = false;
            }    

            //Initialisation du temps de pilotage pour ouvrir/fermer les EV
            if (one_time[8]){
              start_timer_EV1 = millis();
              start_timer_EV2 = millis();
              one_time[8] = false;
            }

            delta_time_EV1 = millis() - start_timer_EV1;
            //Pilotage EV1
            if (delta_time_EV1 <= duree_tot_EV1 && duree_EV1 != 0){
              //Pilotage de EV1 en ouverture
              digitalWrite(8, false);
              digitalWrite(9, true);
            } else {
              //Pilotage de EV1 en fermeture
              digitalWrite(9, false);
              digitalWrite(8, true);
              if (one_time[9] && delta_time_EV1 > duree_tot_EV1 + 9000){
                prod_ok = prod_ok + 1;
                one_time[9] = false;
              }
            }
/*
            //Fermeture EV1
            if (delta_time_EV1 > duree_tot_EV1){
              digitalWrite(8, true);
                if (one_time[3] && duree_tot_EV1 + 9000){
                  prod_ok = prod_ok + 1;
                  one_time[3] = false;
                }
            } else {
              digitalWrite(8, false);
            }
*/
            delta_time_EV2 = millis() - start_timer_EV2;
            //Pilotage EV2
            if (delta_time_EV2 <= duree_tot_EV2 && duree_EV2 != 0){
              //Pilotage de EV1 en ouverture
              digitalWrite(10, false);
              digitalWrite(11, true);
            } else {
              //Pilotage de EV1 en fermeture
              digitalWrite(11, false);
              digitalWrite(10, true);
              if (one_time[10] && delta_time_EV1 > duree_tot_EV1 + 9000){
                prod_ok = prod_ok + 1;
                one_time[10] = false;
              }
            }
/*
            //Fermeture EV2
            if (delta_time_EV2 > duree_tot_EV2){
              digitalWrite(10, true);
              if (one_time[4] && duree_tot_EV1 + 9000){
                prod_ok = prod_ok + 1;
                one_time[4] = false;
              }
            } else {
              digitalWrite(10, false);
            }
*/           
            //Porduction terminée
            if (prod_ok == 2){
              digitalWrite(8, false);
              digitalWrite(9, false);
              digitalWrite(10, false);
              digitalWrite(11, false);
              phase_A = 5;
              prod_ok = 0;
              lcd.clear();
              Serial.print("Production terminée !");
              break;
            }
          
          }

          //Porduction terminée
          while (phase_A == 5){
            index = keyPad.getKey();

            lcd.setCursor(0, 0);
            lcd.print("Production");
            lcd.setCursor(0, 1);
            lcd.print("terminée !");

            //Sortie du menu A
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
