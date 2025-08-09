#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Joystick pin definitions
#define JOYSTICK_X A2   // Connected to VRx
#define JOYSTICK_Y A3   // Connected to VRy
#define JOYSTICK_SW 7   // Connected to SW pin (active LOW)

// nRF24L01 connections
#define CE_PIN 9
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "Free1";

// Data to send: X, Y, SW
struct DataPacket {
  int joyX;    // 0–1023
  int joyY;    // 0–1023
  bool button; // true = pressed
};

DataPacket data;

void setup() {
  Serial.begin(9600);
  
  pinMode(JOYSTICK_SW, INPUT_PULLUP);  // Joystick button

  if (!radio.begin()) {
    Serial.println("nRF24L01 NOT detected!");
    while (1);
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);
  radio.setRetries(0, 15);
  radio.openWritingPipe(address);
  radio.stopListening();

  Serial.println("Transmitter ready");
}

void loop() {
  // Read joystick and button
  data.joyX = analogRead(JOYSTICK_X);
  data.joyY = analogRead(JOYSTICK_Y);
  data.button = !digitalRead(JOYSTICK_SW);  // active LOW

  // Send data
  radio.write(&data, sizeof(data));

  // Debug output
  Serial.print("X: "); Serial.print(data.joyX);
  Serial.print(" | Y: "); Serial.print(data.joyY);
  Serial.print(" | Button: "); Serial.println(data.button);

  delay(50);  // 20–50 ms is typically smooth enough
  delay(100);
}
