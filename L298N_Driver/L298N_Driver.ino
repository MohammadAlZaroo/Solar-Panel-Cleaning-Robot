// Left motor pins
const int IN1 = 35;
const int IN2 = 36;
const int EN  = 37;

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN, OUTPUT);
}

void loop() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(EN, 128);
  delay(5000);

}
