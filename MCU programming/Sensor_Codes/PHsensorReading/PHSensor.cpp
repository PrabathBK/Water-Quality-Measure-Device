#include "PHSensor.h"

float getPHReading(uint8_t AnalogPin) {
    int buffer_arr[10];
    int temp;
    float avgval = 0;

    for (int i = 0; i < 10; i++) {
        buffer_arr[i] = analogRead(AnalogPin);
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

    for (int i = 2; i < 8; i++)
        avgval += buffer_arr[i];

    float volt = (float)avgval * 5 / 1024 / 6;
    float ph_act = 1.7 * volt; //2.8 for arduino && 1.7 for esp8266

    return ph_act;
}
