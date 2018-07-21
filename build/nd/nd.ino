#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <EspSoftwareSerial.h>
#include <math.h>

SoftwareSerial se_read(D5, D6); // write only
SoftwareSerial se_write(D0, D1); // read only

struct ProjectData {
    int32_t is_button_pressed;
    float temp;
    float humi;
    int32_t light_lux;
    float sound;
    int32_t door;
} cur_project_data;

struct ServerData {
    int32_t is_button_pressed;
    float temp;
    float humi;
    int32_t light_lux;
    float sound;
    int32_t door;
} server_data;

const char GET_SERVER_DATA = 1;
const char GET_SERVER_DATA_RESULT = 2;
const char UPDATE_PROJECT_DATA = 3;

// wifi configuration
const char SSID[] = "CHUT";
const char PASSWORD[] = "*123*abc";

// for nodemcu communication
uint32_t last_sent_time = 0;
char expected_data_size = 0;
char cur_data_header = 0;
char buffer[256];
int8_t cur_buffer_length = -1;

void send_to_arduino(char code, void *data, char data_size) {
  char *b = (char*)data;
  int sent_size = 0;
  while (se_write.write(code) == 0) {
    delay(1);
  }
  while (sent_size < data_size) {
    sent_size += se_write.write(b, data_size); //*******
    delay(1);
  }
}

void wifi_initialization() {
  Serial.println("WIFI INITIALIZING.");

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    yield();
    delay(10);
  }

  Serial.println("WIFI INITIALIZED.");
}

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

String set_builder(const char *key, int32_t value) {
  // String str = "http://ku-exceed-backend.appspot.com/api/";
  String str = "http://ecourse.cpe.ku.th:1515/api/"
  str = str + key;
  str = str + "/set/?value=";
  str = str + value;
  return str;
}

//condition for connect server
void update_data_to_server_callback(String const &str) {
  Serial.println("update_data_to_server_callback FINISHED!");
}

bool GET(const char *url, void (*callback)(String const &str,int32_t &value), int32_t &value) {
  HTTPClient main_client;
  main_client.begin(url);
  if (main_client.GET() == HTTP_CODE_OK) {
    Serial.println("GET REQUEST RESPONSE BEGIN");
    if (callback != 0) {
      callback(main_client.getString(),value);
    }
    delay(200);
    return true;
  }
  Serial.println("GET REQUEST RESPONSE BEGIN");
  return false;
}
bool GET(const char *url, void (*callback)(String const &str,float &value), float &value) {
  HTTPClient main_client;
  main_client.begin(url);
  if (main_client.GET() == HTTP_CODE_OK) {
    Serial.println("GET REQUEST RESPONSE BEGIN");
    if (callback != 0) {
      callback(main_client.getString(),value);
    }
    delay(200);
    return true;
  }
  Serial.println("GET REQUEST RESPONSE BEGIN");
  return false;
}

bool POST(const char *url, void (*callback)(String const &str)) {
  HTTPClient main_client;
  main_client.begin(url);
  if (main_client.GET() == HTTP_CODE_OK) {
    Serial.println("GET REQUEST RESPONSE BEGIN");
    if (callback != 0) {
      callback(main_client.getString());
    }
    delay(200);
    return true;
  }
  Serial.println("GET REQUEST RESPONSE BEGIN");
  return false;
}

int get_request_int(String const &str) {
  int32_t tmp = str.toInt();
  return tmp;
}

float get_request_float(String const &str) {
  float tmp = str.toFloat();
  return tmp;
}
void get_request(String const &str, int32_t &value){
  value = get_request_int(str);
}
void get_request(String const &str, float &value){
  value = get_request_float(str);  
}

void get_request_raw_callback(String const &str) {
  Serial.println("GET REQUEST RESPONSE BEGIN");
  Serial.println("========================");
  Serial.println(str.c_str());
  Serial.println("========================");
  Serial.println("GET REQUEST RESPONSE END");
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  serial_initialization();
  wifi_initialization();

  Serial.print("sizeof(ServerData): ");
  Serial.println((int)sizeof(ServerData));
  Serial.print("ESP READY!");
}
void loop() {
  /* 1) Server polling data from server every 1500 ms
     2) Arduino always get local data
  */

  uint32_t cur_time = millis();
  if (cur_time - last_sent_time > 500) {
    //always update
                // int32_t is_button_pressed;
                // float temp;
                // float humi;
                // int32_t light_lux;
                // float sound;
                // int32_t door;

    
    GET("http://ecourse.cpe.ku.th:1515/api/buapalm-is_button_pressed/view/", get_request,server_data.is_button_pressed);
    Serial.print("is_button_pressed : ");
    Serial.println(server_data.is_button_pressed);
    GET("http://ecourse.cpe.ku.th:1515/api/buapalm-light_lux/view/", get_request,server_data.light_lux);
    Serial.print("light_lux : ");
    Serial.println(server_data.light_lux);
    GET("http://ecourse.cpe.ku.th:1515/api/buapalm-door/view/", get_request,server_data.door);
    Serial.print("door : ");
    Serial.println(server_data.door);
    //float variable
    GET("http://ecourse.cpe.ku.th:1515/api/buapalm-temperature/view/", get_request,server_data.temp); 
    Serial.print("temp : ");
    Serial.println(server_data.temp);
    GET("http://ecourse.cpe.ku.th:1515/api/buapalm-humi/view/", get_request,server_data.humi); 
    Serial.print("humi : ");
    Serial.println(server_data.humi);
    GET("http://ecourse.cpe.ku.th:1515/api/buapalm-sound/view/", get_request,server_data.sound); 
    Serial.print("sound : ");
    Serial.println(server_data.sound);
    // GET("http://ecourse.cpe.ku.th:1515/api/exceed-plus/view/", get_request,server_data.plus); 
    // Serial.print("plus : ");
    // Serial.println(server_data.plus);


    last_sent_time = cur_time;
  }

  while (se_read.available()) {
    char ch = se_read.read();
    //Serial.print("RECV: ");0
    //Serial.println((byte)ch);
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
              //depend on what your data is int32_t var = project_data->var;
              //POST(...)
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
                    
                    POST(set_builder("buapalm-humi", humi).c_str(), update_data_to_server_callback);
                    POST(set_builder("buapalm-temp", temp).c_str(), update_data_to_server_callback);
                    POST(set_builder("buapalm-light_lux", light_lux).c_str(), update_data_to_server_callback);
                    POST(set_builder("buapalm-is_button_pressed", is_button_pressed).c_str(), update_data_to_server_callback);
                    POST(set_builder("buapalm-sound", sound).c_str(), update_data_to_server_callback);
                    POST(set_builder("buapalm-door", door).c_str(), update_data_to_server_callback);    
            
            }
            break;
          case GET_SERVER_DATA:
            send_to_arduino(GET_SERVER_DATA_RESULT, &server_data, sizeof(ServerData));
            break;
        }
        cur_buffer_length = -1;
      }
    }
  }
}
