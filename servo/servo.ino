
#include <Servo.h>
#define servo 14
Servo myservo;
void setup() {
  // put your setup code here, to run once:
  myservo.attach(servo);
  myservo.write(0);
//  myservo.write(0);
}

void loop() {
  // put your main code here, to run repeatedly:
  myservo.write(90);
  delay(1000);
  myservo.write(0);
  delay(1000);
//  myservo.write(0);
  
}
