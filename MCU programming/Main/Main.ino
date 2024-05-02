#include <Arduino.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <math.h>


#define API_KEY "AIzaSyCGDSiPxgnVl8zBjkA_UsgWZNUkuS-Ym_I"                                  // Define the API Key
#define DATABASE_URL "https://water-quality-measuring-6f7aa-default-rtdb.firebaseio.com/"  // Database URL

/* Define the user email and password */
#define USER_EMAIL "testdlp@gmail.com"
#define USER_PASSWORD "DLP123"
#define deviceId "HYD00001"


//Include libraries
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin D4 on the Arduino
#define ONE_WIRE_BUS 2  //D4
// int ONE_WIRE_BUS=13;
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);


#define vRef 3.3   // analog reference voltage(Volt) of the ADC
#define sCount 30  // sum of sample point

// int TdsSensorPin=14;
int analogBuffer[sCount];  // store the analog value in the array, read from ADC
int analogBufferTemp[sCount];
int analogBufferIndex = 0;
int copyIndex = 0;

float averageVoltage = 0;
float tdsValue = 0;
float temperature = 23;  // current temperature for compensation
float turbidityNTU = 2.5;
float volt;
float ntu;

bool Switch = false;

// Firebase objects
FirebaseAuth auth;
FirebaseConfig config;
FirebaseData fbdo;

float PHReading = 7.0;
float tdsReading = 0;
float tempReading = 25;
float turbidtyReading = 2.5;



// Thresholds for pH
#define PH_GOOD_LOW 6.5
#define PH_GOOD_HIGH 8.5
#define PH_NORMAL_LOW 6.0
#define PH_NORMAL_HIGH 9.0

// Thresholds for TDS (Total Dissolved Solids) in ppm
#define TDS_GOOD_HIGH 300
#define TDS_NORMAL_HIGH 600

// Thresholds for Temperature in Celsius
#define TEMP_GOOD_LOW 20
#define TEMP_GOOD_HIGH 25
#define TEMP_NORMAL_LOW 15
#define TEMP_NORMAL_HIGH 30

// Thresholds for Turbidity in NTU (Nephelometric Turbidity Units)
#define TURBIDITY_GOOD_HIGH 2.5
#define TURBIDITY_NORMAL_HIGH 3.5


int g1_pin = 4;
int g2_pin = 5;
int a_pin = 9;
int b_pin = 10;
const uint8_t AnalogPin = A0;




void setup() {
  Serial.begin(9600);

  pinMode(g1_pin, OUTPUT);
  pinMode(g2_pin, OUTPUT);
  pinMode(a_pin, OUTPUT);
  pinMode(b_pin, OUTPUT);
  pinMode(AnalogPin, INPUT);

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
  Switch = Firebase.getBool(fbdo, "/RealTimeDB/Tanks/" + String(deviceId) + "/ManSwitch");
  if (Switch) {
    
    multiplexer();
    tempReading = TempSensorReading();
    
    UpdateDB();

    // Determine water quality
    String waterQuality = determineWaterQuality(PHReading, tdsReading, tempReading, turbidtyReading);
    Serial.println(waterQuality);
    Firebase.setString(fbdo, "/RealTimeDB/Tanks/" + String(deviceId) + "/Quality", waterQuality);
  }
}

String determineWaterQuality(float pH, float tds, float temp, float turbidity) {
  if (pH >= PH_GOOD_LOW && pH <= PH_GOOD_HIGH && tds <= TDS_GOOD_HIGH && temp >= TEMP_GOOD_LOW && temp <= TEMP_GOOD_HIGH && turbidity <= TURBIDITY_GOOD_HIGH) {
    return "Good to Use";
  } else if (pH >= PH_NORMAL_LOW && pH <= PH_NORMAL_HIGH && tds <= TDS_NORMAL_HIGH && temp >= TEMP_NORMAL_LOW && temp <= TEMP_NORMAL_HIGH && turbidity <= TURBIDITY_NORMAL_HIGH) {
    return "Normal";
  } else {
    return "Bad to Use";
  }
}





//--------PH sensor----------------

// float calibration_value = 21.34 - 0.7;


void multiplexer(){
  digitalWrite(g1_pin,LOW); //change this to HIGH to get inputs from 2G inputs 
  digitalWrite(g2_pin,HIGH); //change this to LOW to get inputs from 2G inputs 

  //TBD sensor input takes
  digitalWrite(a_pin,LOW);
  digitalWrite(b_pin,LOW);
  turbidtyReading = TurbiditySensorReading();
  delay(1000);

  //TDS sensor input takes
  digitalWrite(a_pin,LOW);
  digitalWrite(b_pin,HIGH);
   tdsReading = TDSSensorReading();
  delay(1000);

  //pH sensor input takes
  digitalWrite(a_pin,HIGH);
  digitalWrite(b_pin,LOW);
  PHReading = PHsensorReading();
  delay(1000);

}



float AVGval;
int BufferArray[10], Temp;

float PHVal = 7;

float PHsensorReading() {

  for (int i = 0; i < 10; i++) {
    BufferArray[i] = analogRead(AnalogPin);
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
  PHVal = 1.7 * Volt;  //2.8 for arduino && 1.7 for esp8266

  Serial.println("pH Val: ");
  Serial.print(PHVal);
  delay(1000);

  return PHVal;
}

void UpdateDB() {

  Firebase.setFloat(fbdo, "/RealTimeDB/Tanks/" + String(deviceId) + "/PH", PHReading);
  Firebase.setFloat(fbdo, "/RealTimeDB/Tanks/" + String(deviceId) + "/Conductivity", tdsReading);
  Firebase.setFloat(fbdo, "/RealTimeDB/Tanks/" + String(deviceId) + "/Temp", tempReading);
  Firebase.setFloat(fbdo, "/RealTimeDB/Tanks/" + String(deviceId) + "/Turbidity", turbidtyReading);
}


float TurbiditySensorReading() {
  volt = 0;
  for (int i = 0; i < 800; i++) {
    volt += ((float)analogRead(AnalogPin) / 1023.0) * 5.0;
  }
  volt = volt / 800;
  volt = round_to_dp(volt, 2);

  if (volt < 2.5) {
    ntu = 3000;
  } else {
    ntu = -1120.4 * square(volt) + 5742.3 * volt - 4353.8;
  }

  Serial.print(volt);
  Serial.print(" V, ");
  Serial.print(ntu);
  Serial.println(" NTU");

  delay(100);
  return ntu;
}

float round_to_dp(float in_value, int decimal_place) {
  float multiplier = powf(10.0f, decimal_place);
  return roundf(in_value * multiplier) / multiplier;
}

float square(float x) {
  return x * x;
}


float TempSensorReading() {
  delay(1000);
  sensors.requestTemperatures();
  Serial.print("Temperature is: ");
  Serial.println(sensors.getTempCByIndex(0));  // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire

  return sensors.getTempCByIndex(0);
}


int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
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
  if ((iFilterLen & 1) > 0) {
    bTemp = bTab[(iFilterLen - 1) / 2];
  } else {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}

float TDSSensorReading() {

  analogBuffer[analogBufferIndex] = analogRead(AnalogPin);  //read the analog value and store into the buffer
  analogBufferIndex++;
  if (analogBufferIndex == sCount) {
    analogBufferIndex = 0;
  }

  for (copyIndex = 0; copyIndex < sCount; copyIndex++) {
    analogBufferTemp[copyIndex] = analogBuffer[copyIndex];

    // read the analog value more stable by the median filtering algorithm, and convert to voltage value
    averageVoltage = getMedianNum(analogBufferTemp, sCount) * (float)vRef / 1024.0;

    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
    //temperature compensation
    float compensationVoltage = averageVoltage / compensationCoefficient;

    //convert voltage value to tds value
    tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;

    Serial.print("TDS Value:");
    Serial.print(tdsValue, 0);
    Serial.println("ppm");
  }
  return tdsValue;
}
