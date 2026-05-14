// ===== Rear Motors (Driver 1) =====
const int IN1 = 4;
const int IN2 = 5;
const int ENA = 6;

const int IN3 = 7;
const int IN4 = 8;
const int ENB = 9;

// ===== Front Motors (Driver 2) =====
const int IN5 = 10; // in1
const int IN6 = 11; // in2
const int ENC = 12; // enA

const int IN7 = 13; // in3
const int IN8 = 14; // in4
const int END = 15; // enb

// PWM channels
const int chA = 0;
const int chB = 1;
const int chC = 2;
const int chD = 3;

// ===== Setup =====
void setup() {
  Serial.begin(115200);

  // Rear motors
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Front motors
  pinMode(IN5, OUTPUT);
  pinMode(IN6, OUTPUT);
  pinMode(IN7, OUTPUT);
  pinMode(IN8, OUTPUT);

  // PWM setup
  ledcSetup(chA, 20000, 8);
  ledcAttachPin(ENA, chA);

  ledcSetup(chB, 20000, 8);
  ledcAttachPin(ENB, chB);

  ledcSetup(chC, 20000, 8);
  ledcAttachPin(ENC, chC);

  ledcSetup(chD, 20000, 8);
  ledcAttachPin(END, chD);
}

// ===== Motor Control =====

// LEFT SIDE (rear + front)
void setLeftMotors(int speed, bool forward) {
  // Rear left
  digitalWrite(IN2, forward ? HIGH : LOW);
  digitalWrite(IN1, forward ? LOW : HIGH);
  ledcWrite(chA, speed);

// Front left
  digitalWrite(IN7, forward ? HIGH : LOW);
  digitalWrite(IN8, forward ? LOW : HIGH);
  ledcWrite(chD, speed);

}

// RIGHT SIDE (rear + front)
void setRightMotors(int speed, bool forward) {
  // Rear right
  digitalWrite(IN3, forward ? HIGH : LOW);
  digitalWrite(IN4, forward ? LOW : HIGH);
  ledcWrite(chB, speed);

  // Front right
  digitalWrite(IN5, forward ? HIGH : LOW);
  digitalWrite(IN6, forward ? LOW : HIGH);
  ledcWrite(chC, speed);
  
}

// ===== Movement Functions =====

void moveForward(int speed) {
  setLeftMotors(speed, false);
  setRightMotors(speed, false);
}

void moveBackward(int speed) {
  setLeftMotors(speed, true);
  setRightMotors(speed, true);
}

void turnLeft(int speed) {
  setLeftMotors(0, false);
  setRightMotors(speed, false);
}

void turnRight(int speed) {
  setLeftMotors(speed, false);
  setRightMotors(0, false);
}

void stopMotors() {
  ledcWrite(chA, 0);
  ledcWrite(chB, 0);
  ledcWrite(chC, 0);
  ledcWrite(chD, 0);
}

// ===== Demo =====

void loop() {
  moveForward(200);
  delay(2000);

  stopMotors();
  delay(1000);

  turnLeft(255);
  delay(1000);

  stopMotors();
  delay(1000);

  moveForward(200);
  delay(2000);

  stopMotors();
  delay(1000);

  turnRight(255);
  delay(1000);

  stopMotors();
  delay(1000);
  
}
