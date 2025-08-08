#include <SPI.h>          // SPI communication library (needed for nRF24L01)
#include <nRF24L01.h>     // nRF24L01 wireless transceiver definitions
#include <RF24.h>         // Main RF24 library for communication

// Motor pin assignments
#define PIN_DIRECTION_RIGHT 3   // Right motor direction control pin
#define PIN_DIRECTION_LEFT  4   // Left motor direction control pin
#define PIN_MOTOR_PWM_RIGHT 5   // Right motor speed control (PWM)
#define PIN_MOTOR_PWM_LEFT  6   // Left motor speed control (PWM)

// Motor noise prevention 
// Solved the issue of the motors making a buzzing noise 
#define DEAD_ZONE 80            

// Radio Communication
RF24 radio(9, 10);               // CE, CSN pins for nRF24L01
const byte address[6] = "Free1"; // Communication channel ID (must match transmitter) - Tutorial of FreeNove

// Structure for received joystick data 
struct Data {
  int xValue;  // Joystick horizontal axis (0–1023)
  int yValue;  // Joystick vertical axis (0–1023)
};
Data data; // Create an instance to store incoming data

// Setup runs once when the Arduino starts 
void setup() {
  Serial.begin(9600); // Serial monitor for debugging

  // Motor pins set as outputs
  pinMode(PIN_DIRECTION_RIGHT, OUTPUT);
  pinMode(PIN_DIRECTION_LEFT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_RIGHT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_LEFT, OUTPUT);
  
  // Initialize the radio for receiving
  radio.begin();
  radio.openReadingPipe(0, address); // Listen on channel "Free1"
  radio.setPALevel(RF24_PA_LOW);     // Low power mode to save energy
  radio.startListening();            // Start listening for incoming data
}


void loop() {

  // Check if data has been received from the transmitter
  if (radio.available()) {
    // Read joystick values into the 'data' structure
    radio.read(&data, sizeof(Data));
    
    // Center joystick values around zero (instead of 0–1023)
    int x = data.xValue - 512; 
    int y = data.yValue - 512;
    
    // Differential steering calculation
    // Left motor speed = forward/backward + turning
    // Right motor speed = forward/backward - turning
    int speedL = y + x;
    int speedR = y - x;
    
    // Limit speeds to the motor driver range (-255 to 255)
    speedL = constrain(speedL, -255, 255);
    speedR = constrain(speedR, -255, 255);
    
    // Apply dead zone
    // If speed is too low, set to 0 to avoid buzzing
    if (abs(speedL) < DEAD_ZONE) speedL = 0;
    if (abs(speedR) < DEAD_ZONE) speedR = 0;
    
    // Control Left Motor 
    if (speedL == 0) {
      // Stop motor
      digitalWrite(PIN_DIRECTION_LEFT, LOW);
      analogWrite(PIN_MOTOR_PWM_LEFT, 0);
    } 
    else if (speedL > 0) {
      // Forward rotation
      digitalWrite(PIN_DIRECTION_LEFT, LOW);
      analogWrite(PIN_MOTOR_PWM_LEFT, speedL);
    } 
    else {
      // Backward rotation
      digitalWrite(PIN_DIRECTION_LEFT, HIGH);
      analogWrite(PIN_MOTOR_PWM_LEFT, -speedL); // Use absolute value
    }
    
    // Control Right Motor 
    if (speedR == 0) {
      // Stop motor
      digitalWrite(PIN_DIRECTION_RIGHT, LOW);
      analogWrite(PIN_MOTOR_PWM_RIGHT, 0);
    } 
    else if (speedR > 0) {
      // Forward rotation
      digitalWrite(PIN_DIRECTION_RIGHT, HIGH);
      analogWrite(PIN_MOTOR_PWM_RIGHT, speedR);
    } 
    else {
      // Backward rotation
      digitalWrite(PIN_DIRECTION_RIGHT, LOW);
      analogWrite(PIN_MOTOR_PWM_RIGHT, -speedR); // Use absolute value
    }
    
    // Debug output to Serial Monitor
    // Useful for ensuring the Control Board with the joystick is responsive 
    Serial.print("X: ");
    Serial.print(data.xValue);
    Serial.print("  Y: ");
    Serial.print(data.yValue);
    Serial.print("  L: ");
    Serial.print(speedL);
    Serial.print("  R: ");
    Serial.println(speedR);
  }
}
