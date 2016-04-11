/*
 * Programme de démo d'un scanner WiFi sur un ESP-01 avec un écran OLED I2C SSD1306
 *  
 * Librairie nécessaire ESP8266Wifi : https://github.com/ekstrand/ESP8266wifi 
 * Gestion de l'affichage sur l'écran OLED d'après le code source de Dan Bick : https://github.com/esp8266/Basic/blob/master/ESP8266Basic/OLED_Functs.ino
 * Plus d'infos sur http://www.projetsdiy.fr/scanner-wifi-ecran-oled-i2c-ssd1306-esp01-esp8266.html
 * Licence : 
 */
#include "ESP8266WiFi.h"
#include <Wire.h>

WiFiClient client;

char buffer[20];                // Buffer pour convertir en chaine de l'adresse IP de l'appareil
char bufrssi[4];                // Buffer pour convertir en chaine la force du signal WiFi
char* ssid     = "SSID";        // Le nom de votre réseau, garder les guillets
char* password = "MOT_DE_PASSE";// Le mot de passe pour se connecter à votre réseau WiFi
bool Connecte = false;          
bool affiche = false;           // Un booléen pour marquer que l'affichage a déjà été fait


#define OLED_address  0x3c      // Adresse de l'écran OLED sur le bus I2C

extern "C" {
  #include "user_interface.h"
}


void setup() {
  Serial.begin(9600);
  delay(2000);                      // Petit délai d'attente
  // Quelques infos système avcant de démarrer
  Serial.print("Heap: "); Serial.println(system_get_free_heap_size());
  Serial.print("Boot Vers: "); Serial.println(system_get_boot_version());
  Serial.print("CPU: "); Serial.println(system_get_cpu_freq());
  Serial.println();
  Wire.pins(0, 2);                  // Brochaes sda, scl sur lequel est branché l'écran OLD SD1306 sur l'ESP-01.
  Wire.begin();
  
  Serial.println("Initialise ecran OLED...");
  StartUp_OLED(); 
  clear_display();
  sendStrXY(" PROJETS DIY ", 0, 1); // 16 caractères par ligne
  sendStrXY(" OLED + ESP-01 ", 2, 1);
  sendStrXY(" DEMARRAGE....  ", 4, 1);
  delay(2000);
  Serial.println("Fin du setup");
}

void Connexion()                   // Fonction pour tester la connexion au réseau Wifi
{
  affiche = false;
  if (WiFi.status() != WL_CONNECTED) {
     WiFi.begin(ssid, password);
     while (WiFi.status() != WL_CONNECTED) {  
         Serial.print(".");
         delay(1000);
     }
  }
}

void loop() {
  Connexion();
  if (!client.connected()) {
      if (!affiche) { 
        affiche = true;
        clear_display();
        
        // On récupère et on prépare le buffer contenant l'adresse IP attibué à l'ESP-01
        IPAddress ip = WiFi.localIP();   
        String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
        ipStr.toCharArray(buffer, 20);

        // On récupère et on prépare le buffer de char contenant la force du signal WiFi
        long rssi = WiFi.RSSI();
        String strRssi;
        strRssi=String(rssi);
        strRssi.toCharArray(bufrssi,4);
        Serial.println(rssi);

        // On actualise l'affichage sur l'écran OLED
        sendStrXY("CONNECTE A", 0, 1);
        sendStrXY((ssid), 3, 1);
        sendStrXY((bufrssi), 3, 10);
        sendStrXY("dBm",3,13),
        sendStrXY((buffer), 6, 1);
      }             
   } else {
      clear_display();
      sendStrXY(" PROJETS DIY ", 0, 1); 
      sendStrXY(" ECHEC CONNEXION ", 2, 1);
      sendStrXY(" SSID ", 4, 1); 
   }
   delay(5000);                         // Délai de 5s entre chaque scanne
}


