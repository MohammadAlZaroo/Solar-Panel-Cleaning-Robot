const int rainAnalog = 4;

void setup() {
  Serial.begin(115200);
  pinMode(rainAnalog, INPUT);
}

void loop() {
  int value = analogRead(rainAnalog);

  Serial.print("Rain level: ");
  Serial.println(value);

  delay(1000);
}
