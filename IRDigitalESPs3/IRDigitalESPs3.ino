const int irPin = 15;

void setup() {
  Serial.begin(115200);
  pinMode(irPin, INPUT);
}

void loop() {
  
Serial.println(digitalRead(irPin));
  delay(500);
}
