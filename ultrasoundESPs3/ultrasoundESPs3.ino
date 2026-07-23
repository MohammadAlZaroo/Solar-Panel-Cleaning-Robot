const int trigPinB = 14;
const int echoPinB = 13;

const int trigPinF = 47;
const int echoPinF = 45;

void setup() {
  Serial.begin(115200);
  pinMode(trigPinB, OUTPUT);
  pinMode(echoPinB, INPUT);

  pinMode(trigPinF, OUTPUT);
  pinMode(echoPinF, INPUT);
}

void loop() {
  front();
  back();
}

void back(){
  // Send trigger pulse
  digitalWrite(trigPinB, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPinB, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinB, LOW);

  // Read echo time
  long durationB = pulseIn(echoPinB, HIGH);

  // Convert to distance (cm)
  float distanceB = durationB * 0.034 / 2;

  Serial.print("DistanceB: ");
  Serial.print(distanceB);
  Serial.println(" cm");

  delay(500);
}


void front(){
  // Send trigger pulse
  digitalWrite(trigPinF, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPinF, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinF, LOW);

  // Read echo time
  long duration = pulseIn(echoPinF, HIGH);

  // Convert to distance (cm)
  float distance = duration * 0.034 / 2;

  Serial.print("DistanceF: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(500);
}
