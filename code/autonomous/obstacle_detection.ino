//We refrenceed the Freenove Tuturial for the obstacle detection code and we built upon the freenove tutorial code.
//Here is the link to download the tutorial: https://freenove.com/fnk0041

#include <Servo.h> 
#define PIN_SERVO           2        
#define MOTOR_DIRECTION     0 //If the direction is reversed, change 0 to 1 
#define PIN_DIRECTION_LEFT  4 
#define PIN_DIRECTION_RIGHT 3 
#define PIN_MOTOR_PWM_LEFT  6 
#define PIN_MOTOR_PWM_RIGHT 5 
 
#define PIN_SONIC_TRIG      7 
#define PIN_SONIC_ECHO      8 
 
#define PIN_BATTERY         A0 
 
// RANGE SETTINGS - Enhanced from smart shield approach
#define OBSTACLE_DISTANCE   60    // Main obstacle detection - increased range
#define OBSTACLE_DISTANCE_LOW 40  // Side obstacle detection - increased  
#define CLEAR_THRESHOLD     80    // Distance for fast turning - increased
 
#define MAX_DISTANCE    2000      
#define SONIC_TIMEOUT   (MAX_DISTANCE*60) 
#define SOUND_VELOCITY    340   

// SPEED SETTINGS - Enhanced with smart shield approach
#define FORWARD_SPEED       150   // Normal forward speed
#define REVERSE_SPEED       255   // Maximum reverse speed  
#define TURN_SPEED          220   // High speed normal turns
#define FAST_TURN_SPEED     255   // Maximum speed fast turns
#define RESUME_SPEED        140   

Servo servo; 
char servoOffset = 0; 
int speedOffset;

// Smart shield style variables
float rightDistance = 0, leftDistance = 0, middleDistance = 0; 
 
void setup() { 
  pinMode(PIN_DIRECTION_LEFT, OUTPUT); 
  pinMode(PIN_MOTOR_PWM_LEFT, OUTPUT); 
  pinMode(PIN_DIRECTION_RIGHT, OUTPUT); 
  pinMode(PIN_MOTOR_PWM_RIGHT, OUTPUT); 
 
  pinMode(PIN_SONIC_TRIG, OUTPUT);
  pinMode(PIN_SONIC_ECHO, INPUT); 
  servo.attach(PIN_SERVO); 
  
  // Start servo at center and keep it there initially
  servo.write(90);
  delay(500);
  
  calculateVoltageCompensation();
  
  Serial.begin(9600);
  Serial.println("Smart Shield Enhanced Robot Ready!");
} 
 
void loop() { 
  updateSmartShieldObstacleAvoidance(); 
} 

// FIXED Smart Shield inspired main logic
void updateSmartShieldObstacleAvoidance() {
  // Calculate the middle distance (smart shield approach)
  middleDistance = getAverageDistance();
  Serial.print("Middle: ");
  Serial.println(middleDistance);

  if(middleDistance <= OBSTACLE_DISTANCE) {
    motorRun(0, 0); // Stop
    delay(200);
    
    // Survey surroundings (smart shield style)
    surveyEnvironment();
    
    // FIXED DECISION LOGIC - Always choose the clearest path
    Serial.print("Left: ");
    Serial.print(leftDistance);
    Serial.print(" Right: ");
    Serial.println(rightDistance);
    
    // Find which side is clearer and by how much
    float leftAdvantage = leftDistance - rightDistance;
    float rightAdvantage = rightDistance - leftDistance;
    float clearestDistance = max(leftDistance, rightDistance);
    
    // Minimum difference required to choose a direction (prevents oscillation)
    float MIN_DIFFERENCE = 15.0; // cm
    
    if(clearestDistance < 30) {
      // Both paths are very blocked - reverse and try again
      Serial.println("Both paths blocked - reversing!");
      motorRun(-(REVERSE_SPEED + speedOffset), -(REVERSE_SPEED + speedOffset));
      delay(300);
      // Turn around 180 degrees
      motorRun((FAST_TURN_SPEED + speedOffset), -(FAST_TURN_SPEED + speedOffset));
      delay(800);
    }
    else if(leftAdvantage > MIN_DIFFERENCE) {
      // Left is significantly clearer
      if(leftDistance > CLEAR_THRESHOLD) {
        Serial.println("LEFT is clearest with clear opening - Fast turn!");
        fastTurnLeft();
        delay(500);
      } else {
        Serial.println("LEFT is clearer - Normal turn");
        normalTurnLeft();
        delay(400);
      }
      motorRun((FORWARD_SPEED + speedOffset), (FORWARD_SPEED + speedOffset));
    }
    else if(rightAdvantage > MIN_DIFFERENCE) {
      // Right is significantly clearer  
      if(rightDistance > CLEAR_THRESHOLD) {
        Serial.println("RIGHT is clearest with clear opening - Fast turn!");
        fastTurnRight();
        delay(500);
      } else {
        Serial.println("RIGHT is clearer - Normal turn");
        normalTurnRight();
        delay(400);
      }
      motorRun((FORWARD_SPEED + speedOffset), (FORWARD_SPEED + speedOffset));
    }
    else {
      // Paths are too similar - default behavior (slight left preference)
      Serial.println("Paths similar - default left turn");
      normalTurnLeft();
      delay(400);
      motorRun((FORWARD_SPEED + speedOffset), (FORWARD_SPEED + speedOffset));
    }
  } 
  else {
    // Move forward
    motorRun((FORWARD_SPEED + speedOffset), (FORWARD_SPEED + speedOffset));
  }
  
  // Return servo to center
  servo.write(90);
  delay(100);
}

// FIXED Smart Shield style environment survey - CORRECTED SERVO DIRECTIONS
void surveyEnvironment() {
  Serial.println("Surveying environment...");
  
  // Look left - FIXED ANGLE
  Serial.println("Checking left...");
  servo.write(150);  // CHANGED: was 30, now 150 for left
  delay(500);
  leftDistance = getAverageDistance();
  Serial.print("Left distance: ");
  Serial.println(leftDistance);

  // Look right - FIXED ANGLE
  Serial.println("Checking right...");
  servo.write(30);   // CHANGED: was 150, now 30 for right
  delay(500);
  rightDistance = getAverageDistance();
  Serial.print("Right distance: ");
  Serial.println(rightDistance);
  
  // Return to center
  servo.write(90);
  delay(300);
}

// Smart Shield style fast turn functions adapted to current motor system
void fastTurnLeft() {
  Serial.println("FAST Turning Left!");
  // Reverse briefly
  motorRun(-(REVERSE_SPEED + speedOffset), -(REVERSE_SPEED + speedOffset));
  delay(150);
  // Fast left turn - 60 degrees
  motorRun(-(FAST_TURN_SPEED + speedOffset), (FAST_TURN_SPEED + speedOffset));
}

void fastTurnRight() {
  Serial.println("FAST Turning Right!");
  // Reverse briefly
  motorRun(-(REVERSE_SPEED + speedOffset), -(REVERSE_SPEED + speedOffset));
  delay(150);
  // Fast right turn - 60 degrees
  motorRun((FAST_TURN_SPEED + speedOffset), -(FAST_TURN_SPEED + speedOffset));
}

// Normal turn functions - 60 degree turns
void normalTurnLeft() {
  Serial.println("Normal Left Turn - 60 degrees");
  motorRun(-(REVERSE_SPEED + speedOffset), -(REVERSE_SPEED + speedOffset));
  delay(150);
  motorRun(-(TURN_SPEED + speedOffset), (TURN_SPEED + speedOffset));
}

void normalTurnRight() {
  Serial.println("Normal Right Turn - 60 degrees");
  motorRun(-(REVERSE_SPEED + speedOffset), -(REVERSE_SPEED + speedOffset));
  delay(150);
  motorRun((TURN_SPEED + speedOffset), -(TURN_SPEED + speedOffset));
}

// Optional: Smart Shield style detailed scanning if needed
void scanningRight() {
  float maxDistance = 0;
  for(int i = 165; i > 90; i -= 5) {
    servo.write(i);
    delay(50);
    float tempDistance = getSonar();
    if(tempDistance > maxDistance) {
      maxDistance = tempDistance;
    }
  }
  rightDistance = maxDistance;
}

void scanningLeft() {
  float maxDistance = 0;
  for(int i = 15; i <= 90; i += 5) {
    servo.write(i);
    delay(50);
    float tempDistance = getSonar();
    if(tempDistance > maxDistance) {
      maxDistance = tempDistance;
    }
  }
  leftDistance = maxDistance;
}
 
// Function to get average distance reading (keeping from original)
int getAverageDistance() {
  int readings = 3; 
  long totalDistance = 0;
  
  for (int i = 0; i < readings; i++) {
    totalDistance += getSonar();
    delay(20); 
  }
  
  return totalDistance / readings;
}
 
float getSonar() { 
  unsigned long pingTime; 
  float distance; 
  
  digitalWrite(PIN_SONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_SONIC_TRIG, HIGH);
  delayMicroseconds(15); 
  digitalWrite(PIN_SONIC_TRIG, LOW); 
  
  pingTime = pulseIn(PIN_SONIC_ECHO, HIGH, SONIC_TIMEOUT);
  if (pingTime != 0) 
    distance = (float)pingTime * SOUND_VELOCITY / 2 / 10000;
  else 
    distance = MAX_DISTANCE; 
  return distance;
} 
 
void calculateVoltageCompensation() {
  float voltageOffset = 8.4 - getBatteryVoltage(); 
  speedOffset = voltageOffset * 25;
} 
 
void motorRun(int speedl, int speedr) { 
  int dirL = 0, dirR = 0; 
  if (speedl > 0) { 
    dirL = 0 ^ MOTOR_DIRECTION; 
  } 
  else { 
    dirL = 1 ^ MOTOR_DIRECTION; 
    speedl = -speedl; 
  } 
  if (speedr > 0) { 
    dirR = 1 ^ MOTOR_DIRECTION; 
  } 
  else { 
    dirR = 0 ^ MOTOR_DIRECTION; 
    speedr = -speedr; 
  } 
  
  speedl = constrain(speedl, 0, 255);
  speedr = constrain(speedr, 0, 255);
  
  digitalWrite(PIN_DIRECTION_LEFT, dirL); 
  digitalWrite(PIN_DIRECTION_RIGHT, dirR); 
  analogWrite(PIN_MOTOR_PWM_LEFT, speedl); 
  analogWrite(PIN_MOTOR_PWM_RIGHT, speedr); 
} 
 
float getBatteryVoltage() { 
  pinMode(PIN_BATTERY, INPUT); 
  int batteryADC = analogRead(PIN_BATTERY); 
  float batteryVoltage = batteryADC / 1023.0 * 5.0 * 4; 
  return batteryVoltage; 
}
