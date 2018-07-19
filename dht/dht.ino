#include <DHT.h>
#define DHPPIN 12
#define DHTTYPE DHT11 
DHT dht(DHPPIN, DHTTYPE);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if(isnan(t) || isnan(h)){
    Serial.println("Failed to read from DHT");  
  }
  else{
    Serial.print("Humidity ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature ");
    Serial.print(t);
    Serial.println("  *c");  
  }
}
