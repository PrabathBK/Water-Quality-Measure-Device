#include <Arduino.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>


#define API_KEY "AIzaSyCGDSiPxgnVl8zBjkA_UsgWZNUkuS-Ym_I"                                       // Define the API Key
#define DATABASE_URL "https://water-quality-measuring-6f7aa-default-rtdb.firebaseio.com/"  // Database URL

/* Define the user email and password */
#define USER_EMAIL "testdlp@gmail.com"
#define USER_PASSWORD "DLP123"
#define deviceId "HYD00001"

#define vRef 3.3              // analog reference voltage(Volt) of the ADC
#define sCount  30            // sum of sample point

// int TdsSensorPin=14;
int analogBuffer[sCount];     // store the analog value in the array, read from ADC
int analogBufferTemp[sCount];
int analogBufferIndex = 0;
int copyIndex = 0;

float averageVoltage = 0;
float tdsValue = 0;
float temperature = 23;       // current temperature for compensation

// Firebase objects
FirebaseAuth auth;
FirebaseConfig config;
FirebaseData fbdo;

float PHReading=7.0;
float tdsReading=0;


const uint8_t AnalogPin=A0;



void setup() {
  Serial.begin(9600);
  //********Connction create**********************************

  WiFiManager wifimanager;
  if (!wifimanager.autoConnect("WaterQualityDeviceConfig")) {
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
  tdsReading=TDSSensorReading();

  UpdateDB();

}





//--------PH sensor----------------

// float calibration_value = 21.34 - 0.7;

float AVGval;
int BufferArray[10], Temp;

float PHVal=7;

float PHsensorReading() {

  for (int i = 0; i < 10; i++) {
    BufferArray[i] = analogRead(A0);
    delay(30);
  }

  //sort the value array using bubble sort
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (BufferArray[i] > BufferArray[j]) {
        Temp = BufferArray[i];
        BufferArray[i] = BufferArray[j];
        BufferArray[j] = Temp;
      }
    }
  }
  AVGval = 0;
  for (int i = 2; i < 8; i++)
    AVGval += BufferArray[i];
  float Volt = AVGval * 5 / 1024 / 6;
  PHVal = 1.7 * Volt; //2.8 for arduino && 1.7 for esp8266

  Serial.println("pH Val: ");
  Serial.print(PHVal);
  delay(1000);

  return PHVal;
}

void UpdateDB(){

  Firebase.setFloat(fbdo,"/RealTimeDB/Tanks/"+String(deviceId)+"/PH",PHReading);
  Firebase.setFloat(fbdo,"/RealTimeDB/Tanks/"+String(deviceId)+"/Conductivity",PHReading);
  

}


int getMedianNum(int bArray[], int iFilterLen){
  int bTab[iFilterLen];
  for (byte i = 0; i<iFilterLen; i++)
  bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0){
    bTemp = bTab[(iFilterLen - 1) / 2];
  }
  else {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}

float TDSSensorReading(){

    analogBuffer[analogBufferIndex] = analogRead(AnalogPin);    //read the analog value and store into the buffer
    analogBufferIndex++;
    if(analogBufferIndex == sCount){ 
      analogBufferIndex = 0;
    }

    for(copyIndex=0; copyIndex<sCount; copyIndex++){
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
      
      // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      averageVoltage = getMedianNum(analogBufferTemp,sCount) * (float)vRef / 1024.0;
      
      //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0)); 
      float compensationCoefficient = 1.0+0.02*(temperature-25.0);
      //temperature compensation
      float compensationVoltage=averageVoltage/compensationCoefficient;
      
      //convert voltage value to tds value
      tdsValue=(133.42*compensationVoltage*compensationVoltage*compensationVoltage - 255.86*compensationVoltage*compensationVoltage + 857.39*compensationVoltage)*0.5;
      
      Serial.print("TDS Value:");
      Serial.print(tdsValue,0);
      Serial.println("ppm");

      
    }
    return tdsValue;
}

