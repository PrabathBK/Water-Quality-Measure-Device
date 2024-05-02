#include <Wire.h>
#include <Arduino.h>  // Often not needed, but ensures compatibility with all Arduino functions.

#include <math.h>  // Include the math library for mathematical functions

int sensorPin = A0;
float volt;
float ntu;

void setup() {
  Serial.begin(115200);

}

void loop() {
  volt = 0;
  for (int i = 0; i < 800; i++) {
    volt += ((float)analogRead(sensorPin) / 1023.0) * 5.0;
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
}

float round_to_dp(float in_value, int decimal_place) {
  float multiplier = powf(10.0f, decimal_place);
  return roundf(in_value * multiplier) / multiplier;
}

float square(float x) {
  return x * x;
}
