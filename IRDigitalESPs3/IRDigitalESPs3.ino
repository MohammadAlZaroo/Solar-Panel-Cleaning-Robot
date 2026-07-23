const int irPin = 11;
//const int irPin = 38;

void setup() {
  Serial.begin(115200);
  pinMode(irPin, INPUT);
}

void loop() {
  
Serial.println(digitalRead(irPin));
  delay(500);
}
