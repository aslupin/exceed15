#include <SoftwareSerial.h>
#include "DHT.h"
#include <Servo.h>
#define LDR 17
#define LED_LDR 2
#define LED_DHT 3
#define BUZZER 4
#define SWITCH 5
#define DHTPIN 8
#define DHTTYPE DHT11
#define SERVO 9

Servo myservo;
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial se_read(12, 13); // write only
SoftwareSerial se_write(10, 11); // read only

struct ProjectData {
  int32_t is_button_pressed;
  float temp;
  float humi;
  int32_t light_lux;
  float sound;
  int32_t door;
} project_data = { 1, 1, 1, 1, 1, 1};

struct ServerData {
  int32_t is_button_pressed;
  float temp;
  float humi;
  int32_t light_lux;
  float sound;
  int32_t door;
} server_data = { 1, 1, 1, 1, 1 , 1};

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
  pinMode(SERVO,OUTPUT);
  while (!se_read.isListening()) {
    se_read.listen();
  }

  Serial.println((int)sizeof(ServerData));
  Serial.println("ARDUINO READY!");
}

uint32_t last_sent_time = 0;
boolean is_data_headers = false;
char expected_data_size = 0;
char cur_data_header = 0;
char buffer[256];
int8_t cur_buffer_length = -1;

void loop() {
  uint32_t cur_time = millis();
  //send to nodemcu
  project_data.light_lux = analogRead(LDR);
  project_data.is_button_pressed = digitalRead(SWITCH);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  project_data.temp = t;
  project_data.humi = h;
  Serial.println("dsffgdgdfsd");
  if (project_data.is_button_pressed == 0) {
    Serial.println("dsfsd");
    analogWrite(BUZZER, HIGH);
    delay(1500);
    analogWrite(BUZZER, LOW);
  }

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
//    Serial.print("RECV: ");
//    Serial.println((byte)ch);
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
              Serial.print("servo: ");
              Serial.println(data->door);
              Serial.print("is_button_pressed");
              Serial.println(data->is_button_pressed);
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
              
              if ((data->humi) == 0 && (data->temp) == 0 ) {
                digitalWrite(LED_DHT, HIGH);
              }
              if ((data->humi) == 1 && (data->temp) == 1 )
              {
                digitalWrite(LED_DHT, LOW);
              }

              
              if ((data->light_lux) == 0 ) {
                digitalWrite(LED_LDR, HIGH);
              }
              if ((data->light_lux) == 1 ) {
                digitalWrite(LED_LDR, LOW);
              }

              
              if ((data->door) == 0 ) {
                  myservo.write(90);
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

