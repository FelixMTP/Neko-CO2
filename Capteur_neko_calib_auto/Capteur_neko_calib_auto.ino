#include "esp_timer.h"
#include <Wire.h>
#include "SCD30.h"
#include "Sensirion_GadgetBle_Lib.h"
#include <EasyBuzzer.h>

// Paramètres à modifier selon le montage électronique
#define SDA_pin 26      // Define the SDA pin used for the SCD30
#define SCL_pin 27      // Define the SCL pin used for the SCD30
int ledR = 19;          // Pin de la led rouge
int ledO = 18;          // Pin de la led orange ou jaune
int ledV = 5;           // Pin de la led verte
int ledB = 17;          // Pin de la led bleue
int pin = 25;           // Pin du Buzzer
int pin_calib = 21;     // Pin de l'interrupteur servant à la calibration 

// Parametres du BUZZER
int nbip = 3;       // Modifier ici le nombre de bip souhaités
int hz = 440;       // Définir ici la fréquence du son souhaité
int duree = 1000;   // Définir ici la durée du bip souhaitée (pas garanti que ça fonctionne en cas de modification)

// Paramètres de la mesure du capteur
int mesure_moyenne_basee_sur = 3; // Fixe la mesure à la moyenne de x mesures pour éviter les fluctuations. Mettre 1 effectuera une mesure non moyennée

// Paramètres des plages de taux de CO2  
float limite_orange = 800;    // Passe les leds et la figure en orange au dela de ce taux
float limite_rouge = 1500;    // Passe les leds et la figure en rouge au dela de ce taux

// Paramètres de calibation manuelle
int test_calib = 10;           // Teste la calibration sur la moyenne de 10 mesures. La valeur moyenne obtenue doit être comprise entre la fourchette haute et la fourchette basse
int fourchette_basse = 390;    // Valeur la plus basse acceptable au test de calibration
int fourchette_haute = 420;    // Valeur la plus hautte acceptable au test de calibration
String cal_passwort = "calib"; // Mot de passe protégeant la calibration
String cal_message  = "";      // Message de calibration
int autocalib = 0;             // Mettre à 1 si vous voulez utiliser la fonction d'autocalibration INTERNE AU CAPTEUR Sensirion scd30 ou 0 si vous voulez utiliser la calibration à taux forcé de ce programme
int val_calib = 400;           // Mettre ici le taux de CO2 servant à la calibration à taux forcé

// Variables à ne pas modifier
int compteur = 0;       // Compteur à ne pas modifier, sert à compter le nombre de bip
static int64_t lastMmntTime = 0;
static int startCheckingAfterUs = 1900000;
GadgetBle gadgetBle = GadgetBle(GadgetBle::DataType::T_RH_CO2_ALT);
float result[3] = {0};
int calibration = 0;    // Marqueur qui indique si on doit effectuer une calibration
  
// Fonction de calibration manuelle
SCD30 airSensorSCD30; // Objekt SDC30 Umweltsensor
void calibrationCO2(int valeur){ // Fonction de calibration
  airSensorSCD30.setForcedRecalibrationFactor(valeur);  // Calibration selon la valeur entrée en paramètre 
  delay(20000);
}

void setup() {
  Serial.begin(115200);
  pinMode(ledR, OUTPUT);
  pinMode(ledO, OUTPUT);
  pinMode(ledV, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(pin_calib, INPUT_PULLDOWN);
  delay(100);
  EasyBuzzer.setPin(pin);
  
  // Initialisation du SCD30
  gadgetBle.begin();
  Serial.print("Identifiant du capteur = ");
  Serial.println(gadgetBle.getDeviceIdString());

  // Initialize the SCD30 driver
  Wire.begin(SDA_pin, SCL_pin);
  
  scd30.initialize();
  scd30.setAutoSelfCalibration(autocalib); // Définit l'autocalibration selon le choix indiqué plus haut
}

void loop() {
  // remise à zéro des variables servant au calcul de la moyenne
  int co2 = 0;
  int temp = 0;
  int hum = 0;
  
  // Controle de l'état du bouton de calibration
  int state_calib = digitalRead(pin_calib);
  Serial.print("calibration : "); Serial.println(state_calib);
  if (state_calib == HIGH) {
    calibration = 1;  
  }
  
  // Si le bouton de calibration est sur ON ou si le test de calibration précédent n'était pas correct, appel de la fonction de calibration au taux de base
  if (calibration == 1) {
    digitalWrite(ledB,LOW);    // on éteint toutes les leds. Elles se rallumeront lors la calibration sera complète.
    digitalWrite(ledV,LOW);
    digitalWrite(ledO,LOW);
    digitalWrite(ledR,LOW);
    Serial.print("Calibration en cours à "); Serial.print(val_calib); Serial.println(" ppm...");
    delay(20000);              // Délai pour laisser le temps de s'éloigner du capteur
    calibrationCO2(val_calib); // Calibration à taux fixé
    Serial.println("Calibration effectuée !");
    Serial.println("Test de calibration...");
    // remise à zéro des variables servant au calcul de la moyenne
    int co2 = 0;
    //Mesures répétées
    for (int i=0; i<test_calib; i++) {
      scd30.getCarbonDioxideConcentration(result);
      Serial.print("CO2[ppm]:"); Serial.println(result[0]);
      delay(2000);
      if (result[0]<10000) {      // Elimination des mesures erronées
        co2 = co2 + result[0];
      } else {
        i--;
      }
    }
    // Calcul de la moyenne
    co2= co2/test_calib;
    Serial.print("Calibration moyenne basée sur ");
    Serial.print(test_calib);
    Serial.println(" mesures.");
    Serial.print("CO2[ppm] moyenne :");
    Serial.println(co2);
    if (co2 > fourchette_basse && co2 < fourchette_haute) {
      calibration = 0;
      Serial.println("Calibration effectuée et testée avec succes !");
      } else {
      Serial.println("La calibration n'est pas correcte.");
    }
    delay(1000);
    }
    delay(2000);
  
  // Mesure du CO2, de la température et du taux d'humidité
  if (esp_timer_get_time() - lastMmntTime >= startCheckingAfterUs && calibration ==0) {
    if (scd30.isAvailable()) {
      //Mesures répétées
      for (int i=0; i<mesure_moyenne_basee_sur; i++) {
        scd30.getCarbonDioxideConcentration(result);
        Serial.print("CO2[ppm]:");
        Serial.print(result[0]);
        Serial.print("\t");
        Serial.print("Temperature[℃]:");
        Serial.print(result[1]);
        Serial.print("\t");
        Serial.print("Humidity[%]:");
        Serial.print(result[2]);
        Serial.println("\t");
        delay(3000);
            co2 = co2 + result[0];
        temp = temp + result[1];
        hum = hum + result[2];
      }
      
      // Calcul de la moyenne
      co2= co2/mesure_moyenne_basee_sur;
      temp= temp/mesure_moyenne_basee_sur;
      hum= hum/mesure_moyenne_basee_sur;
  
      Serial.print("Moyenne basée sur:");
      Serial.print(mesure_moyenne_basee_sur);
      Serial.print("\t");
      Serial.print("CO2[ppm] moyenne :");
      Serial.print(co2);
      Serial.print("\t");
      Serial.print("Temperature[℃] moyenne :");
      Serial.print(temp);
      Serial.print("\t");
      Serial.print("Humidity[%] moyenne :");
      Serial.print(hum);
      Serial.println("\t");
      if (co2 > 400 && co2 < 100000) { // Si la moyenne est en dehors des valeurs plausibles, c'est qu'il y a eu une erreur de mesure
        gadgetBle.writeCO2(co2);
        gadgetBle.writeTemperature(temp);
        gadgetBle.writeHumidity(hum);
        gadgetBle.commit();
        lastMmntTime = esp_timer_get_time();
        // Provide the sensor values for Tools -> Serial Monitor or Serial Plotter
        EasyBuzzer.update();
        if (result[0] < 430){       // S'il y a moins de 430 ppm de CO2
          digitalWrite(ledB,HIGH);    // on allume la led bleue
          digitalWrite(ledV,LOW);
          digitalWrite(ledO,LOW);
          digitalWrite(ledR,LOW);
          compteur =0;                // on remet à zéro le compteur pour que le buzzer puisse sonner à nouveau
        }
        else if (result[0] < 800 && result[0] >= 430){     // S'il y a entre 430 et 800 ppm de CO2
          digitalWrite(ledB,LOW);
          digitalWrite(ledV,HIGH);                           // on allume la led verte
          digitalWrite(ledO,LOW);
          digitalWrite(ledR,LOW);
          compteur =0;                                       // on remet à zéro le compteur pour que le buzzer puisse sonner à nouveau
        }
        else if (result[0] >= 800 && result[0] < 1500){    // S'il y a entre 800 et 1500 ppm de CO2
          digitalWrite(ledB,LOW);
          digitalWrite(ledO,HIGH);                           // on allume la led orange ou jaune
          digitalWrite(ledV,LOW);
          digitalWrite(ledR,LOW);
          // Déplacer ici les lignes correspondant à la sonnerie du buzzer si besoin qu'il sonne dès la limite de 800ppm
    
          // Fin des lignes à déplacer
          // + Commenter la ligne suivante si le buzzer doit sonner dès la limite de 800ppm
          compteur =0; // on remet à zéro le compteur pour que le buzzer puisse sonner à nouveau
        }
        else if (result[0] >= 1500){    // Si on dépasse les 1500 ppm de CO2
          digitalWrite(ledB,LOW);
          digitalWrite(ledR,HIGH);        // on allume la led rouge
          digitalWrite(ledO,LOW);
          digitalWrite(ledV,LOW);
                      // On fait sonner le buzzer

          // Déplacer ces lignes plus haut si le buzzer doit sonner lors du passage de la limite de 800ppm
          if (compteur < nbip) {
            EasyBuzzer.singleBeep(hz, duree);
            compteur = compteur+1;
            delay(1000);
            EasyBuzzer.stopBeep();
          // Fin des lignes à déplacer
          }
        }
      }
    }
    gadgetBle.handleEvents();
    delay(3000);
  }
}
