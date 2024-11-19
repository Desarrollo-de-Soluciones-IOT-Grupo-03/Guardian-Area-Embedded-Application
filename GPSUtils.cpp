#include "GPSUtils.h"
#include <TinyGPS++.h>
#include "JsonUtils.h"
#include "Constants.h"

unsigned long lastGpsUpdateTime = 0;

TinyGPSPlus gps;

void sendGpsDataToServer(){
  if (Serial2.available()) {
    if (gps.encode(Serial2.read())) {
      if (gps.location.isValid()) {
        unsigned long timeSinceLastGpsUpdate = millis() - lastGpsUpdateTime;
        if (timeSinceLastGpsUpdate > 1000) {
          lastGpsUpdateTime = millis();
          String locationPayload = "WS_LOCATION " + jsonify(3, "latitude", String(gps.location.lat()).c_str(), "longitude", String(gps.location.lng()).c_str(),"guardianAreaDeviceRecordId", deviceUUID);;
          Serial3.println(locationPayload);
          //Serial.println(locationPayload);
        }
      }
    }
  }
}