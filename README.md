# Neko+++

Le Neko+++ est un capteur de CO2 avec buzzer et calibration manuelle avec ou sans batterie

auquel j'ai ajouté :
- un buzzer qui sonne un certain nombre de fois.
- une batterie rechargeable
- une fonction de calibration manuelle en extérieur (plus simple) et d’une fonction de calibration manuelle à partir d’un capteur étalonné (plus compliqué)
- trois interrupteurs : pour allumer et éteindre le capteur, pour activer et désactiver le buzzer, pour lancer la calibration

Il est basé sur un mélange :
- du modèle Neko, qui a servi de base : http://lafabrique.centralesupelec.fr/projetco2/#autres
- du modèle Koneko : https://developer.sensirion.com/tutorials/create-your-own-co2-monitor/ et http://lafabrique.centralesupelec.fr/projetco2/#autres
Pour le relevé de mesures du capteur sensirion SCD30, la communication des mesures par Bluetooth et l'application mobile
- du modèle CO2 Ampel : https://github.com/make-IoT/CO2-Ampel/blob/main/CO2-Ampel-DIY-Octopus-SCD30-WEB-AP-CAL.ino
Pour la calibration à une valeur donnée par wifi et la calibration sur l'air extérieur

Fournitures utilisées pour ce modèle (les liens sont indicatifs, les composants identiques ou équivalents peuvent s’acheter ailleurs) :
- 1 Carte ESP32 : https://www.amazon.fr/gp/product/B074RGW2VQ/ (les autres ESP32 sont possible, il faudra juste adapter les paramètres dans l’IDE arduino)
- 2 ou 3 Interrupteurs (2 pour le modèle sans batterie, 3 pour le modèle avec batterie) : https://www.amazon.fr/gp/product/B00O9YQB88/ (d’autres interrupteurs sont possibles aussi mais il faudra peut-être adapter de fichier STL du couvercle)
- 4 Leds : Rouge, jaune, vert, bleu de 20mA
- 4 Résistance 100 ohms
- 1 Capteur : Sensirion SCD30
- 1 Convertisseur d’alimentation (seulement pour le modèle avec batterie) : https://www.amazon.fr/gp/product/B07MY3NZ18
- 1 Module de charge de batterie 18650 (seulement pour le modèle avec batterie) : https://www.amazon.fr/gp/product/B0798M12N8
- 1 Batterie 18650 (seulement pour le modèle avec batterie) : https://www.amazon.fr/gp/product/B08CC5P4HN/
- 1 Support pour batterie (seulement pour le modèle avec batterie) : https://www.amazon.fr/gp/product/B08CY56M98/
