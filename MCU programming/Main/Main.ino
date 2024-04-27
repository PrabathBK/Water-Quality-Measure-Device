#include <Arduino.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

#define API_KEY "AIzaSyCGDSiPxgnVl8zBjkA_UsgWZNUkuS-Ym_I"                                       // Define the API Key
#define DATABASE_URL "https://water-quality-measuring-6f7aa-default-rtdb.firebaseio.com/"  // Database URL

/* Define the user email and password */
#define USER_EMAIL "testdlp@gmail.com"
#define USER_PASSWORD "DLP123"
#define DEVICE_ID "HYD0001"

// Firebase objects
FirebaseAuth auth;
FirebaseConfig config;
FirebaseData fbdo;


void setup() {
  Serial.begin(9600);
  //********Connction create**********************************

  WiFiManager wifimanager;
  if (!wifimanager.autoConnect("HydrolinkConfig")) {
    Serial.println("Failed to connect and hit timeout");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // config.token_status_callback = tokenStatusCallback;  // see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  Serial.println("Connected to Firebase!");
  //*********************************************************//


}
void loop() {

}



