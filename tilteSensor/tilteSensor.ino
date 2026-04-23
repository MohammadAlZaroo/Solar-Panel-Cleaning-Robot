const int tiltPin = 18;

void setup() {
  Serial.begin(115200);
  pinMode(tiltPin, INPUT);
}

void loop() {
  int state = digitalRead(tiltPin);

  if (state == LOW) {
    Serial.println("📐 Tilt detected!");
  } else {
    Serial.println("✔ Stable");
  }

  delay(500);
}
