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

float PHReading=7.0;


const uint8_t AnalogPin=A0;


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

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  Serial.println("Connected to Firebase!");
  //*********************************************************//


}
void loop() {

  PHReading=PHsensorReading();

}





//--------PH sensor----------------

// float calibration_value = 21.34 - 0.7;
int phval = 0;
unsigned long int avgval;
int buffer_arr[10], temp;

float ph_act;

float PHsensorReading() {

  for (int i = 0; i < 10; i++) {
    buffer_arr[i] = analogRead(A0);
    delay(30);
  }
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buffer_arr[i] > buffer_arr[j]) {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }
  avgval = 0;
  for (int i = 2; i < 8; i++)
    avgval += buffer_arr[i];
  float volt = (float)avgval * 5 / 1024 / 6;
  ph_act = 1.7 * volt; //2.8 for arduino && 1.7 for esp8266

  Serial.println("pH Val: ");
  Serial.print(ph_act);
  delay(1000);

  return ph_act;
}