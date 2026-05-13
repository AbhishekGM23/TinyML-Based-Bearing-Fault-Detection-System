#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#define SDA_PIN 8
#define SCL_PIN 9

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

void setup() {
  Serial.begin(115200);
  delay(3000);  // important for ESP32-S3 USB

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!accel.begin()) {
    Serial.println("Sensor not detected");
    while (1);
  }

  accel.setRange(ADXL345_RANGE_2_G);
   Serial.println("x,y,z");
}

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);

  // Send ONLY CSV (Edge Impulse requirement)
  Serial.print(event.acceleration.x);
  Serial.print(",");
  Serial.print(event.acceleration.y);
  Serial.print(",");
  Serial.println(event.acceleration.z);

  delay(10);  // ~100 Hz sampling
}