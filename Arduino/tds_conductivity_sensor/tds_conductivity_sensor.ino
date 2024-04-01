#include <algorithm>

#define TdsSensorPin 34
#define VREF 3.3              // analog reference voltage of the ADC
#define SCOUNT  30            // sum of sample point
#define RELAY_PIN_1 18
#define RELAY_PIN_2 19

int analogBuffer[SCOUNT];     // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
int copyIndex = 0;

float averageVoltage = 0;     // Floats to calculate average voltage & TDS value
float tdsValue = 0;
float temperature = 25; 

int getMedianNum(int bArray[], int iFilterLen) {
  // Declare an array bTab with size iFilterLen
  int bTab[iFilterLen];
  // Copy elements from bArray to bTab
  for (int i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  
  // Sort the elements in bTab
  std::sort(bTab, bTab + iFilterLen);
  
  // Calculate the median value based on the sorted array bTab
  int median;
  if ((iFilterLen & 1) > 0) {
    // If the length of the array is odd, the median is the middle element
    median = bTab[(iFilterLen - 1) / 2];
  }
  else {
    // If the length of the array is even, the median is the average of the two middle elements
    median = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  
  // Return the median value
  return median;
}

void setup() {
  Serial.begin(115200);  // Initialize serial communication with a baud rate of 115200
  pinMode(TdsSensorPin, INPUT);  // Set TdsSensorPin as input
  pinMode(RELAY_PIN_1, OUTPUT);  // Set RELAY_PIN_1 as output
  pinMode(RELAY_PIN_2, OUTPUT);  // Set RELAY_PIN_2 as output
}

void loop() {
  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U) { //every 40 milliseconds, read the analog value from the ADC
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT) {
      analogBufferIndex = 0;
    }
  }

  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U) {
    printTimepoint = millis();
    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++) {
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];

      // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 4096.0;

      //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);

      //temperature compensation
      float compensationVoltage = averageVoltage / compensationCoefficient;

      //convert voltage value to tds value
      tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;

      if (tdsValue < 1000) {
        Serial.print("TDS Value Low:");
        Serial.print(tdsValue, 0);
        Serial.println("ppm");

        // turn on pump A & B for 1 seconds
        digitalWrite(RELAY_PIN_1, LOW);
        digitalWrite(RELAY_PIN_2, LOW);
        delay(1000);

        digitalWrite(RELAY_PIN_1, HIGH);
        digitalWrite(RELAY_PIN_2, HIGH);
        delay(1000);
      }
      else if (tdsValue == 1000) {
        Serial.print("TDS Value:");
        Serial.print(tdsValue, 0);
        Serial.println("ppm");
      }
      else {
        // Do nothing for other cases
      }
    }
  }
}
