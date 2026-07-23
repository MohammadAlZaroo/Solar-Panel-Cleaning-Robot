#include <WiFi.h>
#include <esp_now.h>
#include <IskakINO_LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(16, 2);

const int broom = 37;
const int pumbPin = 40;


// Rear Left motor
const int L_IN1 = 16;
const int L_IN2 = 15;
const int L_EN  = 17;

// Rear Right motor
const int R_IN1 = 11;
const int R_IN2 = 10;
const int R_EN  = 46;

// Front Left motor
const int FL_IN1 = 5;
const int FL_IN2 = 4;
const int FL_EN  = 6;

// Front Right motor
const int FR_IN1 = 8;
const int FR_IN2 = 7;
const int FR_EN  = 9;

typedef struct {
  int vertical;
  int horizontal;
  int speedLeft;
  int speedRight;
} JoystickData;

JoystickData receivedData;

unsigned long lastReceiveTime = 0;
const unsigned long timeout = 500;

String currentStatus = "";

void displayStatus(String line1, String line2) {
  String newStatus = line1 + line2;

  if (newStatus != currentStatus) {
    currentStatus = newStatus;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);

    lcd.setCursor(0, 1);
    lcd.print(line2);
  }
}

void initiateLcd() {
  lcd.begin();
  lcd.backlight();

  displayStatus("Starting Robot", "ESP32-S3 Ready");
}

void setup() {
  Serial.begin(115200);
  Wire.begin(1, 2);
  pinMode(broom, OUTPUT);
  pinMode(pumbPin, OUTPUT);
  initiateLcd();

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

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    displayStatus("ESP-NOW Failed", "Check Code");
    return;
  }

  esp_now_register_recv_cb(onDataReceive);

  Serial.println("ESP32-S3 ESP-NOW Receiver Ready");
  displayStatus("ESP-NOW Ready", "Waiting Signal");
}

void loop() {
  if (millis() - lastReceiveTime > timeout) {
    stopRobot();
  }
}

void onDataReceive(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));

  lastReceiveTime = millis();

  int leftSpeed = receivedData.speedLeft;
  int rightSpeed = receivedData.speedRight;

  if (leftSpeed == 0 && rightSpeed == 0) {
    digitalWrite(broom, LOW);
    digitalWrite(pumbPin, LOW);
    
  } else {
    digitalWrite(broom, HIGH);
    digitalWrite(pumbPin, HIGH);
  }

  driveLeftSide(leftSpeed);
  driveRightSide(rightSpeed);

  updateMovementDisplay(leftSpeed, rightSpeed);

  Serial.print("Left: ");
  Serial.print(leftSpeed);
  Serial.print(" | Right: ");
  Serial.println(rightSpeed);
}

void updateMovementDisplay(int leftSpeed, int rightSpeed) {
  if (leftSpeed == 0 && rightSpeed == 0) {
    displayStatus("Stopped", "Cleaning");
  }
  else if (leftSpeed > 0 && rightSpeed > 0) {
    displayStatus("Cleaning", "Moving Forward");
  }
  else if (leftSpeed < 0 && rightSpeed < 0) {
    displayStatus("Cleaning", "Moving Backward");
  }
  else if (leftSpeed > 0 && rightSpeed < 0) {
    displayStatus("Cleaning", "Turning Left");
  }
  else if (leftSpeed < 0 && rightSpeed > 0) {
    displayStatus("Cleaning", "Turning Right");
  }
  else {
    displayStatus("Cleaning", "Moving");
  }
}

void driveLeftSide(int speedValue) {
  speedValue = constrain(speedValue, -255, 255);

  if (speedValue > 0) {
    digitalWrite(L_IN1, HIGH);
    digitalWrite(L_IN2, LOW);
    digitalWrite(FL_IN1, HIGH);
    digitalWrite(FL_IN2, LOW);
  }
  else if (speedValue < 0) {
    digitalWrite(L_IN1, LOW);
    digitalWrite(L_IN2, HIGH);
    digitalWrite(FL_IN1, LOW);
    digitalWrite(FL_IN2, HIGH);
  }
  else {
    digitalWrite(L_IN1, LOW);
    digitalWrite(L_IN2, LOW);
    digitalWrite(FL_IN1, LOW);
    digitalWrite(FL_IN2, LOW);
  }

  analogWrite(L_EN, abs(speedValue));
  analogWrite(FL_EN, abs(speedValue));
}

void driveRightSide(int speedValue) {
  speedValue = constrain(speedValue, -255, 255);

  if (speedValue > 0) {
    digitalWrite(R_IN1, HIGH);
    digitalWrite(R_IN2, LOW);
    digitalWrite(FR_IN1, HIGH);
    digitalWrite(FR_IN2, LOW);
  }
  else if (speedValue < 0) {
    digitalWrite(R_IN1, LOW);
    digitalWrite(R_IN2, HIGH);
    digitalWrite(FR_IN1, LOW);
    digitalWrite(FR_IN2, HIGH);
  }
  else {
    digitalWrite(R_IN1, LOW);
    digitalWrite(R_IN2, LOW);
    digitalWrite(FR_IN1, LOW);
    digitalWrite(FR_IN2, LOW);
  }

  analogWrite(R_EN, abs(speedValue));
  analogWrite(FR_EN, abs(speedValue));
}

void stopRobot() {
  digitalWrite(broom, LOW);
  digitalWrite(pumbPin, LOW);
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

  displayStatus("Stopped", "Cleaning");
}
