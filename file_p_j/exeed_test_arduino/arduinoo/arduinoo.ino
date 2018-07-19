#include <SoftwareSerial.h>

SoftwareSerial se_read(12, 13); // write only
SoftwareSerial se_write(10, 11); // read only

struct ProjectData {
  int32_t is_button_pressed;
  float temp;
  int32_t light_lux;
  float sound;
  int32_t door;
} project_data = { 1, 30.11988, 360 , -651.654, 0};

struct ServerData {
  int32_t is_button_pressed;
  float temp;
  int32_t light_lux;
  float sound;
  int32_t door;
} server_data = { 0, 0, 0, 0, 0 };

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
  pinMode(LED_BUILTIN, OUTPUT);
  while (!se_read.isListening()) {
    se_read.listen();
  }

  Serial.println((int)sizeof(ServerData));
  Serial.println("ARDUINO READY!");

}

uint32_t last_sent_time = 0;

void loop() {
  // put your main code here, to run repeatedly:
  uint32_t cur_time = millis();
  //send to nodemcu
  if (cur_time - last_sent_time > 50) {//always update
    send_to_nodemcu(UPDATE_PROJECT_DATA, &project_data, sizeof(ProjectData));
//    send_to_nodemcu(GET_SERVER_DATA, &server_data, sizofe(ServerData));
    last_sent_time = cur_time;
  }

}
