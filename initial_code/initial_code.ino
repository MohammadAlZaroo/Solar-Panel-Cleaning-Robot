#include "thingProperties.h"
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <LiquidCrystal_I2C.h>

// ===================== MPU6050 + LCD =====================

Adafruit_MPU6050 mpu;
LiquidCrystal_I2C lcd(0x27, 16, 2);   // Change 0x27 to 0x3F if your LCD address is different

// ===================== Motor pins =====================

// Left front motor - old left motor
const int LF_IN1 = 15;
const int LF_IN2 = 16;
const int LF_EN  = 17;

// Right front motor - old right motor
const int RF_IN1 = 41;
const int RF_IN2 = 42;
const int RF_EN  = 46;

// Left rear motor - CHANGE THESE PINS
const int LR_IN1 = 4;
const int LR_IN2 = 5;
const int LR_EN  = 6;

// Right rear motor - CHANGE THESE PINS
const int RR_IN1 = 7;
const int RR_IN2 = 8;
const int RR_EN  = 9;

// ===================== Relay pins =====================

const int WATER_RELAY = 10;   // Change this pin
const int BROOM_RELAY = 11;   // Change this pin

// If your relay module turns ON when the input is LOW, keep this true.
// If your relay turns ON when the input is HIGH, change it to false.
const bool RELAY_ACTIVE_LOW = true;

// ===================== Ultrasonic sensor pins =====================

// Front ultrasonic
const int FRONT_TRIG = 12;    // Change this pin
const int FRONT_ECHO = 13;    // Change this pin

// Back ultrasonic
const int BACK_TRIG = 18;     // Change this pin
const int BACK_ECHO = 19;     // Change this pin

// ===================== IR sensor pins =====================

const int LEFT_IR  = 20;      // Change this pin
const int RIGHT_IR = 21;      // Change this pin

// Many IR modules output LOW when they detect something.
// If your IR sensor gives HIGH when it detects the panel/edge, change this.
const int IR_DETECTED = LOW;

// ===================== Physical switch =====================

const int ON_OFF_SWITCH = 47; // Change this pin

// If using INPUT_PULLUP:
// switch ON  = connected to GND = LOW
// switch OFF = open = HIGH
const int SWITCH_ON = LOW;

// ===================== Cleaning settings =====================

const int CLEAN_DISTANCE_CM = 5;
const int DEFAULT_SPEED = 150;

const unsigned long TURN_TIME = 900;       // milliseconds
const unsigned long SENSOR_UPDATE = 200;   // milliseconds
const unsigned long LCD_UPDATE = 500;      // milliseconds

// ===================== Variables =====================

float frontDistance = 0;
float backDistance = 0;

bool leftIrDetected = false;
bool rightIrDetected = false;

unsigned long lastSensorRead = 0;
unsigned long lastLcdUpdate = 0;

enum RobotState {
  STOPPED,
  CLEAN_FORWARD,
  TURN_LEFT,
  CLEAN_BACKWARD,
  TURN_RIGHT
};

RobotState robotState = STOPPED;
unsigned long stateStartTime = 0;


// ===================== Setup =====================

void setup() {
  Serial.begin(9600);
  delay(1500);

  // Motor pins
  pinMode(LF_IN1, OUTPUT);
  pinMode(LF_IN2, OUTPUT);
  pinMode(LF_EN, OUTPUT);

  pinMode(RF_IN1, OUTPUT);
  pinMode(RF_IN2, OUTPUT);
  pinMode(RF_EN, OUTPUT);

  pinMode(LR_IN1, OUTPUT);
  pinMode(LR_IN2, OUTPUT);
  pinMode(LR_EN, OUTPUT);

  pinMode(RR_IN1, OUTPUT);
  pinMode(RR_IN2, OUTPUT);
  pinMode(RR_EN, OUTPUT);

  // Relay pins
  pinMode(WATER_RELAY, OUTPUT);
  pinMode(BROOM_RELAY, OUTPUT);
  waterPumpOff();
  broomOff();

  // Ultrasonic pins
  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);

  pinMode(BACK_TRIG, OUTPUT);
  pinMode(BACK_ECHO, INPUT);

  // IR pins
  pinMode(LEFT_IR, INPUT);
  pinMode(RIGHT_IR, INPUT);

  // Physical ON/OFF switch
  pinMode(ON_OFF_SWITCH, INPUT_PULLUP);

  stopRobot();

  // I2C
  Wire.begin();

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Solar Cleaner");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  // MPU6050
  if (!mpu.begin()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MPU6050 Error");
    Serial.println("Failed to find MPU6050");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Arduino IoT Cloud
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Robot Ready");
}


// ===================== Loop =====================

void loop() {
  ArduinoCloud.update();

  readSensorsPeriodically();
  updateCleaningLogic();
  updateLcdPeriodically();
}


// ===================== Motor control =====================

void moveMotor(int in1, int in2, int en, bool direction, int motorSpeed) {
  motorSpeed = constrain(motorSpeed, 0, 255);

  if (direction) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }

  analogWrite(en, motorSpeed);
}

void stopMotor(int in1, int in2, int en) {
  analogWrite(en, 0);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

void moveLeftSide(bool direction, int motorSpeed) {
  moveMotor(LF_IN1, LF_IN2, LF_EN, direction, motorSpeed);
  moveMotor(LR_IN1, LR_IN2, LR_EN, direction, motorSpeed);
}

void moveRightSide(bool direction, int motorSpeed) {
  moveMotor(RF_IN1, RF_IN2, RF_EN, direction, motorSpeed);
  moveMotor(RR_IN1, RR_IN2, RR_EN, direction, motorSpeed);
}

void moveForward(int motorSpeed) {
  moveLeftSide(true, motorSpeed);
  moveRightSide(true, motorSpeed);
}

void moveBackward(int motorSpeed) {
  moveLeftSide(false, motorSpeed);
  moveRightSide(false, motorSpeed);
}

void turnLeft(int motorSpeed) {
  moveLeftSide(false, motorSpeed);
  moveRightSide(true, motorSpeed);
}

void turnRight(int motorSpeed) {
  moveLeftSide(true, motorSpeed);
  moveRightSide(false, motorSpeed);
}

void stopRobot() {
  stopMotor(LF_IN1, LF_IN2, LF_EN);
  stopMotor(RF_IN1, RF_IN2, RF_EN);
  stopMotor(LR_IN1, LR_IN2, LR_EN);
  stopMotor(RR_IN1, RR_IN2, RR_EN);
}


// ===================== Relay control =====================

void relayWrite(int relayPin, bool state) {
  if (RELAY_ACTIVE_LOW) {
    digitalWrite(relayPin, state ? LOW : HIGH);
  } else {
    digitalWrite(relayPin, state ? HIGH : LOW);
  }
}

void waterPumpOn() {
  relayWrite(WATER_RELAY, true);
}

void waterPumpOff() {
  relayWrite(WATER_RELAY, false);
}

void broomOn() {
  relayWrite(BROOM_RELAY, true);
}

void broomOff() {
  relayWrite(BROOM_RELAY, false);
}

void cleaningToolsOn() {
  waterPumpOn();
  broomOn();
}

void cleaningToolsOff() {
  waterPumpOff();
  broomOff();
}


// ===================== Sensor reading =====================

float readUltrasonicCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);

  if (duration == 0) {
    return 999;   // no reading
  }

  float distance = duration * 0.0343 / 2.0;
  return distance;
}

void readMPUAngle() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Roll angle approximation from accelerometer
  float roll = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;

  // Send angle value to Arduino Cloud variable
  angle = roll;
}

void readSensorsPeriodically() {
  if (millis() - lastSensorRead >= SENSOR_UPDATE) {
    lastSensorRead = millis();

    frontDistance = readUltrasonicCM(FRONT_TRIG, FRONT_ECHO);
    backDistance  = readUltrasonicCM(BACK_TRIG, BACK_ECHO);

    leftIrDetected  = digitalRead(LEFT_IR) == IR_DETECTED;
    rightIrDetected = digitalRead(RIGHT_IR) == IR_DETECTED;

    readMPUAngle();

    Serial.print("Front: ");
    Serial.print(frontDistance);
    Serial.print(" cm | Back: ");
    Serial.print(backDistance);
    Serial.print(" cm | L_IR: ");
    Serial.print(leftIrDetected);
    Serial.print(" | R_IR: ");
    Serial.print(rightIrDetected);
    Serial.print(" | Angle: ");
    Serial.println(angle);
  }
}


// ===================== Cleaning logic =====================

bool robotEnabled() {
  bool physicalSwitchOn = digitalRead(ON_OFF_SWITCH) == SWITCH_ON;

  if (startStop == true && physicalSwitchOn == true) {
    return true;
  }

  return false;
}

int getCleaningSpeed() {
  int motorSpeed = constrain((int)speed, 0, 255);

  if (motorSpeed == 0) {
    motorSpeed = DEFAULT_SPEED;
  }

  return motorSpeed;
}

void setState(RobotState newState) {
  robotState = newState;
  stateStartTime = millis();
}

void updateCleaningLogic() {
  if (!robotEnabled()) {
    stopRobot();
    cleaningToolsOff();
    setState(STOPPED);
    return;
  }

  int motorSpeed = getCleaningSpeed();

  // Safety using side IR sensors
  // If left side sensor says danger/no panel, turn right
  if (!leftIrDetected) {
    cleaningToolsOn();
    turnRight(motorSpeed);
    setState(TURN_RIGHT);
    return;
  }

  // If right side sensor says danger/no panel, turn left
  if (!rightIrDetected) {
    cleaningToolsOn();
    turnLeft(motorSpeed);
    setState(TURN_LEFT);
    return;
  }

  switch (robotState) {
    case STOPPED:
      cleaningToolsOn();
      setState(CLEAN_FORWARD);
      break;

    case CLEAN_FORWARD:
      cleaningToolsOn();

      // As you requested:
      // while front ultrasonic reading is below 5 cm, clean forward
      if (frontDistance < CLEAN_DISTANCE_CM) {
        moveForward(motorSpeed);
      } else {
        setState(TURN_LEFT);
      }
      break;

    case TURN_LEFT:
      cleaningToolsOn();
      turnLeft(motorSpeed);

      if (millis() - stateStartTime >= TURN_TIME) {
        setState(CLEAN_BACKWARD);
      }
      break;

    case CLEAN_BACKWARD:
      cleaningToolsOn();

      // Same logic for back ultrasonic
      if (backDistance < CLEAN_DISTANCE_CM) {
        moveBackward(motorSpeed);
      } else {
        setState(TURN_RIGHT);
      }
      break;

    case TURN_RIGHT:
      cleaningToolsOn();
      turnRight(motorSpeed);

      if (millis() - stateStartTime >= TURN_TIME) {
        setState(CLEAN_FORWARD);
      }
      break;
  }
}


// ===================== LCD =====================

void updateLcdPeriodically() {
  if (millis() - lastLcdUpdate >= LCD_UPDATE) {
    lastLcdUpdate = millis();

    lcd.clear();

    lcd.setCursor(0, 0);

    if (!robotEnabled()) {
      lcd.print("Status: OFF");
    } else {
      switch (robotState) {
        case STOPPED:
          lcd.print("Status: STOP");
          break;

        case CLEAN_FORWARD:
          lcd.print("Clean Forward");
          break;

        case TURN_LEFT:
          lcd.print("Turning Left");
          break;

        case CLEAN_BACKWARD:
          lcd.print("Clean Backward");
          break;

        case TURN_RIGHT:
          lcd.print("Turning Right");
          break;
      }
    }

    lcd.setCursor(0, 1);
    lcd.print("F:");
    lcd.print((int)frontDistance);
    lcd.print(" B:");
    lcd.print((int)backDistance);
    lcd.print(" A:");
    lcd.print((int)angle);
  }
}


// ===================== Arduino IoT Cloud callbacks =====================

void onSpeedChange() {
  Serial.print("Speed changed to: ");
  Serial.println(speed);
}

void onAngleChange() {
  Serial.print("Angle changed to: ");
  Serial.println(angle);
}

void onStartStopChange() {
  Serial.print("Start/Stop changed to: ");
  Serial.println(startStop);
}

void onBattaryChargeChange() {
  Serial.print("Battery charge: ");
  Serial.println(battaryCharge);
}
