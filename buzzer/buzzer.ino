#define buzzer 11
void setup() {
  // put your setup code here, to run once:
  pinMode(buzzer, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(buzzer, HIGH);
  delay(1000);
  analogWrite(buzzer, LOW);
  delay(1000);
}
