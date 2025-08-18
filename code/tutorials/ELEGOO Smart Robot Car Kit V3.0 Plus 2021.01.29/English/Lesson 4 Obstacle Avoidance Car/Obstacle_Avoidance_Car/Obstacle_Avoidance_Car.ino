#include <Servo.h>  // Servo library
Servo myservo;      // Create servo object

// Motor driver pins (Freenove Smart Car Shield v1.1)
// #define IN1 7
// #define IN2 8
// #define ENA 5
// #define IN3 9
// #define IN4 11
// #define ENB 6

// changing the motor pins
// Determine if the motors are active/moving or not
// set this pin to high to run the motors
#define STBY 3

// LEFT MOTORS
#define PWMA 5
#define AIN1 7
#define AIN2 4  

// RIGHT MOTORS
#define PWMB 6
#define BIN1 8
#define BIN2 9     

// setting motor speed
int MotorSpeed = 150;

// Ultrasonic sensor
#define Trig 13
#define Echo 12

#define carSpeed 200

float rightDistance = 0, leftDistance = 0, middleDistance = 0;

// calculates the distance 
float Distance_test() {
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);

  float duration = pulseIn(Echo, HIGH, 30000);
  
  if (duration == 0) {
    // No echo received (timeout)
    Serial.println("Nothing recieved");
    return 999;
  }

  float Fdistance = duration * 0.034 / 2;
  return Fdistance;
}

void setup() {
  Serial.begin(9600);

  // the following pins will be set as output 
  pinMode(STBY, OUTPUT);  
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);   
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT); 

  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);

  myservo.attach(10); // Attach servo to pin 10 (connected to P10 on Freenove)
  myservo.write(90);  // Center the servo
  stop();
}

void loop() {

  // setting the servo in the center position
  myservo.write(90);
  delay(500);

  // counter that keeps track of the different angles
  int counterRight = 0, counterLeft = 0;

  // counter that keeps track of the different angles
  float accumRight = 0, accumLeft = 0;

  // Calculate the middle distance
  middleDistance = Distance_test();
  Serial.print("Middle: ");
  Serial.println(middleDistance);

  // Once an object is encountered straight ahead
  if (middleDistance <= 30) {
    stop();
    delay(250);

    Serial.println("Distance is less 30cm...");

    // go into a for loop that goes to the right
    for(int i = 15; i <= 165; i+=2.5)
    {
      myservo.write(i);
      delay(30);
      rightDistance = Distance_test();
      accumRight += rightDistance;
      Serial.println(rightDistance);

      counterRight++;
    }

    // going the opposite way back to center
    for(int i = 165; i >90; i-=2.5)
    {
      myservo.write(i);
      delay(30);
      leftDistance = Distance_test();

      accumLeft += leftDistance;
      Serial.println(leftDistance);

      counterLeft++;
    }

    //taking the average of each direction
    float avgRightDist = accumRight / counterRight;
    float avgLeftDist = accumLeft / counterLeft;

    Serial.println("Averages: ");
    
    Serial.println("Average of Right: ");
    Serial.println(avgRightDist);

    Serial.println("Average of Left: ");
    Serial.println(avgLeftDist);

    // Calculating the distance
    delay(1000);

  } else {
    forward();
  }

  delay(100);
  }


// --------- Motor Functions --------- //

void forward() {
  Serial.println("Forward");

  digitalWrite(STBY, HIGH);
  
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  
  analogWrite(PWMA, MotorSpeed);
  analogWrite(PWMB, MotorSpeed);

}

void back() {
  Serial.println("Reverse");

  digitalWrite(STBY, HIGH);
  
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  
  analogWrite(PWMA, MotorSpeed);
  analogWrite(PWMB, MotorSpeed);

}

void right() {
  Serial.println("Turning right");

  digitalWrite(STBY, HIGH);

  // Left motor forward
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  
  // Right motor reverse
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  
  analogWrite(PWMA, MotorSpeed);
  analogWrite(PWMB, MotorSpeed);

}

void left() {
  Serial.println("Turning Left");

  digitalWrite(STBY, HIGH);

  // Left motor reverse
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  
  // Right motor forward
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  
  analogWrite(PWMA, MotorSpeed);
  analogWrite(PWMB, MotorSpeed);

}

void stop() {
  Serial.println("Stop");
  digitalWrite(STBY, LOW);
}