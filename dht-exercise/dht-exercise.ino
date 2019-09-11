#include <dht11.h>
dht11 DHT;

void setup() {
  Serial.begin(9600);
}

void loop() {
  DHT.read(10);
  Serial.println("");
  Serial.print("Temperature:");
  Serial.print(DHT.temperature,1);
  Serial.print(", humidity: ");
  Serial.print(DHT.humidity,1);
  delay(1000);
}
