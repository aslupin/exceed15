#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <EspSoftwareSerial.h>
#include <math.h>

SoftwareSerial se_read(D5, D6); // write only
SoftwareSerial se_write(D0, D1); // read only

struct ServerData {
  int32_t is_button_pressed;
  float temp;
  int32_t light_lux;
  float sound;
  int32_t door;
} server_data;

struct ProjectData {
  int32_t is_button_pressed;
  float temp;
  int32_t light_lux;
  float sound;
  int32_t door;
} cur_project_data;

const char GET_SERVER_DATA = 1;
const char GET_SERVER_DATA_RESULT = 2;
const char UPDATE_PROJECT_DATA = 3;

uint32_t last_sent_time = 0;
char expected_data_size = 0;
char cur_data_header = 0;
char buffer[256];
int8_t cur_buffer_length = -1;



void serial_initialization() {
  Serial.begin(115200);
  se_read.begin(38400);
  se_write.begin(38400);

  while (!se_read.isListening()) {
    se_read.listen();
  }

  Serial.println();
  Serial.println("SERIAL INITIALIZED.");
}

void setup() {
  // put your setup code here, to run once:
  serial_initialization();
  Serial.print("sizeof(ServerData): ");
  Serial.println((int)sizeof(ServerData));
  Serial.print("ESP READY!");

}

void loop() {
  // put your main code here, to run repeatedly:
  while (se_read.available()) {
      char ch = se_read.read();
      if (cur_buffer_length == -1) {
        cur_data_header = ch;
        switch (cur_data_header) {
          case UPDATE_PROJECT_DATA:
            expected_data_size = sizeof(ProjectData);
            cur_buffer_length = 0;
            break;
          case GET_SERVER_DATA:
            expected_data_size = sizeof(ServerData);
            cur_buffer_length = 0;
            break;
        }
       } else if (cur_buffer_length < expected_data_size) {
         buffer[cur_buffer_length++] = ch;
         if (cur_buffer_length == expected_data_size) {
            switch (cur_data_header) {
                case UPDATE_PROJECT_DATA: {
                    ProjectData *project_data = (ProjectData*)buffer;
                    float temp = project_data->temp;
                    int32_t light_lux = project_data->light_lux;
                    int32_t is_button_pressed = project_data->is_button_pressed;
                    float sound = project_data->sound;
                    int32_t door = project_data->door;
                    Serial.print("Temp : ");
                    Serial.println(temp);
                    Serial.print("light_lux : ");
                    Serial.println(light_lux);
                    Serial.print("is_button_pressed : ");
                    Serial.println(is_button_pressed);
                    Serial.print("sound : ");
                    Serial.println(sound);
                    Serial.print("door : ");
                    Serial.println(door);
                    break;
                }
            }
            cur_buffer_length = -1;
         }
       }
    }
}
