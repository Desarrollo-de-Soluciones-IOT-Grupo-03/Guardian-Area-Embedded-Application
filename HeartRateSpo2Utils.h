#ifndef HEARTRATESPO2UTILS_H
#define HEARTRATESPO2UTILS_H
#include "MAX30105.h"

// Initializes sensor with default parameters
void initParticleSensor(MAX30105 &sensor);

// Reads sensor data
void readSensorData(MAX30105 &sensor, uint32_t *redBuffer, uint32_t *irBuffer, int bufferLength);

// Calculates BPM and SPO2
void calculateHeartRateAndSpO2(uint32_t *irBuffer, uint32_t *redBuffer, int bufferLength, 
                               int32_t *spo2, int8_t *validSPO2, int32_t *heartRate, int8_t *validHeartRate);

// Shows the calculated results
void displayResults(int32_t spo2, int8_t validSPO2, int32_t heartRate, int8_t validHeartRate);

//Sends BPM and SpO2 to the server
void sendHealthDataToServer(MAX30105 &sensor);

#endif