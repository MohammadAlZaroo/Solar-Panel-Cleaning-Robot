// Rear Left motor
const int L_IN1 = 16; // in
const int L_IN2 = 15; 
const int L_EN  = 17;

// Rear Right motor
const int R_IN1 = 11; // in3
const int R_IN2 = 10; // in4
const int R_EN  = 46; // enb

// Front Left motor
const int FL_IN1 = 5;
const int FL_IN2 = 4;
const int FL_EN  = 6;

// Front Right motor
const int FR_IN1 = 8;
const int FR_IN2 = 7;
const int FR_EN  = 9;

void setup() {
  Serial.begin(115200);
  // Set all pins as outputs
  pinMode(L_IN1, OUTPUT);
  pinMode(L_IN2, OUTPUT);
  pinMode(L_EN, OUTPUT);

  pinMode(R_IN1, OUTPUT);
  pinMode(R_IN2, OUTPUT);
  pinMode(R_EN, OUTPUT);

  pinMode(FL_IN1, OUTPUT);
  pinMode(FL_IN2, OUTPUT);
  pinMode(FL_EN, OUTPUT);

  pinMode(FR_IN1, OUTPUT);
  pinMode(FR_IN2, OUTPUT);
  pinMode(FR_EN, OUTPUT);

  stopRobot();
}

void loop() {
  moveForward(255);  // Full speed
  Serial.println("forward");
  delay(3000);       // Move for 3 seconds

  stopRobot();
    Serial.println("Stop");

  delay(3000);       // Stop for 3 seconds
}

void moveForward(int speedValue) {
  // Rear Left
  digitalWrite(L_IN1, HIGH);
  digitalWrite(L_IN2, LOW);
  analogWrite(L_EN, speedValue);

  // Rear Right
  digitalWrite(R_IN1, HIGH);
  digitalWrite(R_IN2, LOW);
  analogWrite(R_EN, speedValue);

  // Front Left
  digitalWrite(FL_IN1, HIGH);
  digitalWrite(FL_IN2, LOW);
  analogWrite(FL_EN, speedValue);

  // Front Right
  digitalWrite(FR_IN1, HIGH);
  digitalWrite(FR_IN2, LOW);
  analogWrite(FR_EN, speedValue);
}

void stopRobot() {
  analogWrite(L_EN, 0);
  analogWrite(R_EN, 0);
  analogWrite(FL_EN, 0);
  analogWrite(FR_EN, 0);

  digitalWrite(L_IN1, LOW);
  digitalWrite(L_IN2, LOW);

  digitalWrite(R_IN1, LOW);
  digitalWrite(R_IN2, LOW);

  digitalWrite(FL_IN1, LOW);
  digitalWrite(FL_IN2, LOW);

  digitalWrite(FR_IN1, LOW);
  digitalWrite(FR_IN2, LOW);
}

void turnRight(int speedValue) {
  // Left side backward
  digitalWrite(L_IN1, LOW);
  digitalWrite(L_IN2, HIGH);
  analogWrite(L_EN, speedValue);

  digitalWrite(FL_IN1, LOW);
  digitalWrite(FL_IN2, HIGH);
  analogWrite(FL_EN, speedValue);

  // Right side forward
  digitalWrite(R_IN1, HIGH);
  digitalWrite(R_IN2, LOW);
  analogWrite(R_EN, speedValue);

  digitalWrite(FR_IN1, HIGH);
  digitalWrite(FR_IN2, LOW);
  analogWrite(FR_EN, speedValue);
}

void turnLeft(int speedValue) {
  // Left side forward
  digitalWrite(L_IN1, HIGH);
  digitalWrite(L_IN2, LOW);
  analogWrite(L_EN, speedValue);

  digitalWrite(FL_IN1, HIGH);
  digitalWrite(FL_IN2, LOW);
  analogWrite(FL_EN, speedValue);

  // Right side backward
  digitalWrite(R_IN1, LOW);
  digitalWrite(R_IN2, HIGH);
  analogWrite(R_EN, speedValue);

  digitalWrite(FR_IN1, LOW);
  digitalWrite(FR_IN2, HIGH);
  analogWrite(FR_EN, speedValue);
}
