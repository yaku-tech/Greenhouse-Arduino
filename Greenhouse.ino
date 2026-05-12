// =========================================================================================================================================================
// Скетч для автоматизации теплицы в огороде
//

#include "DHT.h"
#include "Adafruit_Sensor.h"

void setup() {
  // put your setup code here, to run once:
  dht.begin();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  float h = dht.readHumidity();  // влажность воздуха
  float t = dht.readTemperature(); // температура воздуха

  Serial.print("Humidity: ");
  Serial.println(h);
  Serial.print("Temperature: ");
  Serial.println(t);

  delay(5000);
}
