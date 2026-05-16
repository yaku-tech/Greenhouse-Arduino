// =========================================================================================================================================================
// Скетч для автоматизации теплицы в огороде
//

// =========================================================================================================================================================

#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 3 (on the right) of the sensor to GROUND (if your sensor has 3 pins)
// Connect pin 4 (on the right) of the sensor to GROUND and leave the pin 3 EMPTY (if your sensor has 4 pins)
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

DHT_Unified dht_u(DHTPIN, DHTTYPE);

// =========================================================================================================================================================

#include <Arduino.h>
#include <GyverDS3231.h>

// Initialize TimeStamp sensor.
GyverDS3231 rtc;

String dateTimeValue;

// =========================================================================================================================================================

struct DataPoint {
  int id;
  String datetime;  
  float humid;      
  float temp;       
  
  DataPoint(int i, String d, float h, float t) {
    id = i;
    datetime = d;
    humid = h;
    temp = t;
  }
};

const int MAX_ITEMS = 12;
DataPoint* collection[MAX_ITEMS];
int itemCount = 0;

void setup() {

  Serial.begin(9600);

  initializeTemperatureSensor();

  initializeStampSensor();

}

void loop() {

  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  dateTimeValue = rtc.toString();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  delay(1000);
  
  Serial.print(F("ID: "));
  Serial.print(itemCount);
  Serial.print(F("    Date and time: "));
  Serial.print(dateTimeValue);  // rtc.timeToString(), rtc.dateToString()
  Serial.print(F("    Humidity: "));
  Serial.print(h);
  Serial.print(F("%    Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F   Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));

  delay(1000);
  
  // Adding in array
  addToCollection(itemCount, dateTimeValue, h, t);

  // Incrementing
  itemCount++;
  if (itemCount >= MAX_ITEMS) {  
    itemCount = 0;
  }

  if (Serial.available()) {
    char value = Serial.read();
    if (value == '1') { 
      Serial.println("----------------------- Saved data -----------------------");
      for (int i = 0; i < MAX_ITEMS; i++) {
        int id = collection[i]->id;
        String datetime = collection[i]->datetime;
        float temp = collection[i]->temp;

        Serial.print("id - " + String(id) + " time - " + datetime + " temp - " + String(temp));

        Serial.println();
        delay(200);
      }
      Serial.println("----------------------- Continue -------------------------");
    }
  }

}

void addToCollection(int k0, String k1, float k2, float k3) {
  delete collection[k0];
  collection[k0] = nullptr;

  DataPoint* item = new DataPoint(k0, k1, k2, k3);
  collection[itemCount] = item;

  delay(200);
}

// =========================================================================================================================================================

void initializeTemperatureSensor() {
  // Print temperature sensor details.
  sensor_t sensor;
  dht_u.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht_u.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));

  dht.begin();
}

void initializeStampSensor() {
  // Get date and time from stamp sensor 
  setStampZone(3); // часовой пояс
  Wire.begin();
  rtc.begin();

  Serial.print("OK: ");
  Serial.println(rtc.isOK());

  Serial.print("Reset: ");
  Serial.println(rtc.isReset());

  if (rtc.isReset()) {
      rtc.setBuildTime();  // установить время компиляции прошивки
      // rtc.setTime(2025, 1, 30, 12, 45, 0); // установить время вручную
  }
}

// =========================================================================================================================================================
