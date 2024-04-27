


// float calibration_value = 21.34 - 0.7;
int phval = 0;
unsigned long int avgval;
int buffer_arr[10], temp;

float ph_act;
// for the OLED display

//#define SCREEN_WIDTH 128 // OLED display width, in pixels
//#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
//#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  //Wire.begin();
  Serial.begin(9600);
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  //display.clearDisplay();
  //display.setTextColor(WHITE);


  //timer.setInterval(500L, display_pHValue);
}
void loop() {
  //timer.run(); // Initiates SimpleTimer
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
}