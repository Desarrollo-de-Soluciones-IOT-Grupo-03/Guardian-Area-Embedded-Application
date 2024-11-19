#pragma once
#include <Arduino.h>

void sendDataFromMainSerialToEsp();
String getEspResponse();
void sendDeviceID();
void requestApiKeyToServer();
void sendApiKeyToEsp(String apiKey);
String saveApiKeyServerResponse(String espResponse);
void printEspResponse(String espResponse);