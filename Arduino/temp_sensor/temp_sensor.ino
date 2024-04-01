#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>

#define Temp_sensor 3

OneWire oneWire (Temp_sensor);

DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  sensors.begin();
  pinMode(Temp_sensor, INPUT);

}

void loop() {
  sensors.requestTemperatures();
  Serial.print("Temp: ");
  Serial.print(sensors.getTempCByIndex(0));
  Serial.print("\n");
  delay(1000);
}