#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//This code was inspired by the YouTube video: turbidity sensor arduino ||Turbidity meter arduino project.
//By Skynet Robotics, the link to the video: https://www.youtube.com/watch?v=nT6Q5atNLJM
//This code was also inpsired by the Youtube video: Determine Water Quality using Arduino and Turbidity Sensor | DIY Turbidity meter.
//By Muhammad Ansar, the link to the video: https://www.youtube.com/watch?v=MytUqOz5vbY 

// LCD setup - adjust address if needed (common addresses: 0x27, 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SENSOR_PIN A0

int read_ADC;
int ntu;

void setup() {
  // Initialize serial for debugging (optional)
  Serial.begin(9600);

  // Configure sensor pin
  pinMode(SENSOR_PIN, INPUT);

  // Initialize LCD
  lcd.init();        // Initialize the LCD
  lcd.backlight();   // Turn on backlight

  // Welcome message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Welcome To  ");
  lcd.setCursor(0, 1);
  lcd.print("Turbidity Sensor");
  delay(3000);
  lcd.clear();
}

void loop() {
  // Read sensor value
  read_ADC = analogRead(SENSOR_PIN);

  // Limit maximum reading and map to NTU
  if(read_ADC > 208) read_ADC = 208;
  ntu = map(read_ADC, 0, 208, 300, 0);

  // Clear both lines to prevent leftover characters
  lcd.clear();

  // Display turbidity value on first line
  lcd.setCursor(0, 0);
  lcd.print("Turbidity: ");
  lcd.print(ntu);
  lcd.print(" NTU");

  // Display water quality on second line
  lcd.setCursor(0, 1);
  if(ntu < 10) {
    lcd.print("Very Clean Water");
  }
  else if(ntu >= 10 && ntu < 30) {
    lcd.print("Clean Water     ");  // Added spaces to clear line
  }
  else {
    lcd.print("Dirty Water     ");  // Added spaces to clear line
  }

  // Optional: Print to Serial Monitor for debugging
  Serial.print("ADC Reading: ");
  Serial.print(read_ADC);
  Serial.print(" | NTU: ");
  Serial.println(ntu);

  delay(500);  // Slightly longer delay for better readability
}