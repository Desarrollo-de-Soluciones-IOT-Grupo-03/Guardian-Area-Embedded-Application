#include "HeartRateSpO2Utils.h"
#include "spo2_algorithm.h"
#include "JsonUtils.h"
#include "Constants.h"


void initParticleSensor(MAX30105 &sensor) {
  if (!sensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("ERROR: MAX30105 not found");
    return;
  }
  byte ledBrightness = 70;  // RED LED Brightness (0=off, 255=max)
  byte sampleAverage = 4;   // avg samples
  byte ledMode = 2;         // 1 = RED only, 2 = RED + IR
  byte sampleRate = 400;    // Sampling Frequency (Hz)
  int pulseWidth = 411;
  int adcRange = 4096;

  sensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}

void readSensorData(MAX30105 &sensor, uint32_t *redBuffer, uint32_t *irBuffer, int bufferLength) {
  static int index = 0;  // Llevar el índice como variable estática
  if (index < bufferLength) {
    if (sensor.available()) {
      redBuffer[index] = sensor.getRed();
      irBuffer[index] = sensor.getIR();
      sensor.nextSample();
      index++;
    } else {
      sensor.check();
    }
  } else {
    index = 0;
  }
}

void calculateHeartRateAndSpO2(uint32_t *irBuffer, uint32_t *redBuffer, int bufferLength,
                               int32_t *spo2, int8_t *validSPO2, int32_t *heartRate, int8_t *validHeartRate) {
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, spo2, validSPO2, heartRate, validHeartRate);
}

void displayResults(int32_t spo2, int8_t validSPO2, int32_t heartRate, int8_t validHeartRate) {
  if (validHeartRate && validSPO2) {
    Serial.print(F("BPM: "));
    Serial.print(heartRate);
    Serial.print(F(", SpO2: "));
    Serial.println(spo2);
  }
}

void sendHealthDataToServer(MAX30105 &sensor) {
  static uint32_t irBuffer[50];
  static uint32_t redBuffer[50];
  static int32_t spo2 = 0;
  static int8_t validSPO2 = 0;
  static int32_t heartRate = 0;
  static int8_t validHeartRate = 0;

  readSensorData(sensor, redBuffer, irBuffer, 50);
  calculateHeartRateAndSpO2(irBuffer, redBuffer, 50, &spo2, &validSPO2, &heartRate, &validHeartRate);
  //displayResults(spo2, validSPO2, heartRate, validHeartRate);

  if (validHeartRate && validSPO2) {
    String heartRateStr = (String)heartRate;
    String spo2Str = (String)spo2;
    String healthPayload = jsonify(3, "bpm", heartRateStr.c_str(), "spo2", spo2Str.c_str(), "guardianAreaDeviceRecordId", deviceUUID);

    String healthDataCmd = "WS_HEALTH " + healthPayload;
    //Serial.println(healthDataCmd);
    Serial3.println(healthDataCmd);
  }
}