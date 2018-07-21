#include <SoftwareSerial.h>
#include "DHT.h"
#include <Servo.h>
#define LDR 17
#define LED_LDR 2
#define LED_DHT 3
#define BUZZER 9
#define SWITCH 5
#define DHTPIN 8
#define DHTTYPE DHT11
#define SERVO 14
#define triggerpin_back 0
#define echopin_back 1
#define triggerpin_fro 7
#define echopin_fro 6

//long duration, cm, floor_length;
//long microsecondsToCentimeters(long microseconds)
//{
//  return microseconds / 29 / 2;
//}

Servo myservo;
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial se_read(12, 13); // write only
SoftwareSerial se_write(10, 11); // read only

struct ProjectData {
  int32_t is_button_pressed;  //switch
  float temp;                 //temperature --dht
  float humi;                 //humidity --dht
  int32_t light_lux;          //ldr
  float sound;                //buzzer
  int32_t door;               //servo
  int32_t frontultra;          //ultrasonic --frontdoor
  int32_t backultra;           //ultrasonic --backdoor
  int32_t openair;
} project_data = { 1, 1, 1, 1, 1, 1, 1, 1, 1};

struct ServerData {
  int32_t is_button_pressed;
  float temp;
  float humi;
  int32_t light_lux;
  float sound;
  int32_t door;
  int32_t frontultra;
  int32_t backultra;
  int32_t openair;
} server_data = { 1, 1, 1, 1, 1, 1, 1, 1, 1};

const char GET_SERVER_DATA = 1;
const char GET_SERVER_DATA_RESULT = 2;
const char UPDATE_PROJECT_DATA = 3;

void send_to_nodemcu(char code, void *data, char data_size) {
  char *b = (char*)data;
  char sent_size = 0;
  while (se_write.write(code) == 0) {
    delay(1);
  }
  while (sent_size < data_size) {
    sent_size += se_write.write(b, data_size);
    delay(1);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  se_read.begin(38400);
  se_write.begin(38400);
  pinMode(LDR, INPUT);
  pinMode(LED_LDR, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(SWITCH, INPUT);
  pinMode(LED_DHT, OUTPUT);
  pinMode(SERVO, OUTPUT);
  myservo.attach(SERVO);
  myservo.write(0);
//  pinMode(triggerpin_fro,INPUT);
//  digitalWrite(triggerpin_fro,LOW);
//  delayMicroseconds(2);
//  digitalWrite(triggerpin_fro,HIGH);
//  delayMicroseconds(5);
//  digitalWrite(triggerpin_fro,LOW);
//  pinMode(echopin_fro,INPUT);
//  duration = pulseIn(echopin_fro,HIGH);
//  floor_lenght = microsecondsToCentimaters(duration);
  
  while (!se_read.isListening()) {
    se_read.listen();
  }

  Serial.println((int)sizeof(ServerData));
  Serial.println("ARDUINO READY!");
}

uint32_t last_sent_time = 0;
boolean is_data_header = false;
char expected_data_size = 0;
char cur_data_header = 0;
char buffer[256];
int8_t cur_buffer_length = -1;

void loop() {
  uint32_t cur_time = millis();
  //send to nodemcu
//  project_data.light_lux = analogRead(LDR);
  //project_data.is_button_pressed = digitalRead(SWITCH);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
    project_data.temp = t;
  //  project_data.humi = h;

  /*////////////////  กดกริ่งเอง ข้อ1 ////////////////////*/
  if (digitalRead(SWITCH) == 0) {
//    Serial.println("hgfh");
    analogWrite(BUZZER, tone);
    delay(1500);
    analogWrite(BUZZER, LOW);
  }

  /*//////////////// อุณหภูมิสูง แอร์เปิด ข้อ1advance////////////////*/
  /*            if ((project.data.humi > 60)) {
                  digitalWrite(LED_DHT, HIGH);
                }
                if ((project.data.humi <= 60))
                {
                  digitalWrite(LED_DHT, LOW);
                }*/


  /*//////////////// เชคldr แล้วเปิดไฟ ข้อ2advance////////////////*/
  /*            if ((project_data.light_lux) > 100) {
                  digitalWrite(LED_LDR, HIGH);
                }
                if ((project_data.light_lux) > 100)
                {
                  digitalWrite(LED_LDR, LOW);
                }*/

  /*//////////////// ประตูเปิดอัตโนมัติ ข้อ3advance////////////////*/
  /*            if ((project_data.ultra) < 30) {
                    myservo.write(110);
                    delay(1000);
                }
                if ((project_data.ultra) >= 30)
                {
                    myservo.write(0);
                }*/

  /*//////////////// เปิดแอร์อัตโนมัติ ข้อ4advance////////////////*/
  /*            if ((project_data.smartair) < 30) {
                    myservo.write(110);
                    delay(1000);
                }
                if ((project_data.ultra) >= 30)
                {
                    myservo.write(0);
                }*/


  if (cur_time - last_sent_time > 500) {//always update
    send_to_nodemcu(UPDATE_PROJECT_DATA, &project_data, sizeof(ProjectData));
    send_to_nodemcu(GET_SERVER_DATA, &server_data, sizeof(ServerData));
    last_sent_time = cur_time;
  }


  //read from sensor....

  //send to nodemcu


  //read data from server pass by nodemcu
  while (se_read.available()) {
    char ch = se_read.read();
    //Serial.print("RECV: ");
    //Serial.println((byte)ch);
    if (cur_buffer_length == -1) {
      cur_data_header = ch;
      switch (cur_data_header) {
        case GET_SERVER_DATA_RESULT:
          //unknown header
          expected_data_size = sizeof(ServerData);
          cur_buffer_length = 0;
          break;
      }
    } else if (cur_buffer_length < expected_data_size) {
      buffer[cur_buffer_length++] = ch;
      if (cur_buffer_length == expected_data_size) {
        switch (cur_data_header) {
          case GET_SERVER_DATA_RESULT: {
              ServerData *data = (ServerData*)buffer;
              //use data to control sensor
              Serial.print("light: ");
              Serial.println(data->light_lux);
              Serial.print("sound status: ");
              Serial.println(data->sound);
              Serial.print("ultra sonic: ");
              Serial.println(data->door);
              Serial.print("is_button_pressed: ");
              Serial.println(data->is_button_pressed);
              Serial.print("openair: ");
              Serial.println(data->openair);
              if (isnan(t) || isnan(h)) {
                Serial.println("Failed to read from DHT");
              } else {
                Serial.print("Humidity: ");
                Serial.print(h);
                Serial.print(" \t");
                Serial.print("Temperature: ");
                Serial.print(t);
                Serial.println(" *C");
              }

              /*///////////////////// กดกริ่งจาก serv ข้อ1 soft////////////////////////*/
              if ((data->is_button_pressed) == 0) {
                digitalWrite(BUZZER, HIGH);
              }
              if ((data->is_button_pressed) == 1) 
              {
                digitalWrite(BUZZER, LOW);
              }

              /*///////////////////// เปิด/ปิดเเอร์เองจาก serv ข้อ 2 /////////////*/
              if ((data->openair) == 0 ) {
                digitalWrite(LED_DHT, HIGH);
              }
              if ((data->openair) == 1 )
              {
                digitalWrite(LED_DHT, LOW);
              }

              /*///////////////////// เปิด/ปิด ไฟจาก serv ข้อ3 ////////////////*/
              if ((data->light_lux) == 0 ) {
                digitalWrite(LED_LDR, HIGH);
              }
              if ((data->light_lux) == 1 ) {
                digitalWrite(LED_LDR, LOW);
              }

              /*///////////////////// เปิด/ปิด ประตูจาก serv ข้อ4 //////////////*/
              if ((data->door) == 0 ) {
                myservo.write(110);
                delay(2500);
              }
              if ((data->door) == 1 ) {
                myservo.write(0);
              }

            } break;
        }
        cur_buffer_length = -1;
      }
    }
  }
}

