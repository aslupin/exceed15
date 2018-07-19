#define trigger_pin 7
#define echo_pin 6
long duration,cm,floor_lenght;
long microsecondsToCentimaters(long microseconds){
  return   microseconds / 29 / 2;
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(trigger_pin,INPUT);
  digitalWrite(trigger_pin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigger_pin,HIGH);
  delayMicroseconds(5);
  digitalWrite(trigger_pin,LOW);
  pinMode(echo_pin,INPUT);
  duration = pulseIn(echo_pin,HIGH);
  floor_lenght = microsecondsToCentimaters(duration);
  
}

void loop() {
  // put your main code here, to run repeatedly:
//  pinMode(trigger_pin,OUTPUT);
  digitalWrite(trigger_pin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigger_pin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin,LOW);
//  pinMode(echo_pin, INPUT);
  duration = pulseIn(echo_pin, HIGH);
  cm = microsecondsToCentimaters(duration);
  Serial.print(cm);
  Serial.println(" cm");
  
  
  
}
