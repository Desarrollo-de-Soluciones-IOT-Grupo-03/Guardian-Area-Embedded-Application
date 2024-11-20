#include "Arduino.h"
#include "AccelerometerUtils.h"
#include <Adafruit_ADXL345_U.h>
#include "JsonUtils.h"
#include "Constants.h"

unsigned long lastAccelerometerUpdateTime = 0;

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

void initAccelerometer() {
  if (!accel.begin()) {
    Serial.println("ERROR: ADXL345 not found.");
  }

  accel.setRange(ADXL345_RANGE_16_G);
}

void sendAccelerometerDataToServer() {
  if (millis() - lastAccelerometerUpdateTime > 1000) {
    lastAccelerometerUpdateTime = millis();
    sensors_event_t event;
    accel.getEvent(&event);

    String accelerometerCmd = "WS_ACCELEROMETER" + jsonify(4,
                                                          "X", String(event.acceleration.x).c_str(),
                                                          "Y", String(event.acceleration.y).c_str(),
                                                          "Z", String(event.acceleration.z).c_str(),
                                                          "guardianAreaDeviceRecordId", deviceUUID);
    //Serial3.println(accelerometerCmd);
  }
}