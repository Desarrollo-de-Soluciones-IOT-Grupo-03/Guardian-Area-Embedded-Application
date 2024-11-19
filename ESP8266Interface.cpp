#include "ESP8266Interface.h"
#include "Constants.h"
#include <SPI.h>
#include <SD.h>

void sendDataFromMainSerialToEsp() {
  // Send data from main serial monitor to ESP serial (Serial 3)
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    Serial3.println(command);
    Serial.println(command);
  }
}

String getEspResponse() {
  String response = "";
  if (Serial3.available()) {
    response = Serial3.readStringUntil('\n');
    response.trim();
  }
  return response;
}

void sendDeviceID() {
  String uuidCommand = "DEVICE_ID " + (String)deviceUUID;  //Send UUID to ESP
  Serial3.println(uuidCommand);
  Serial.println(uuidCommand);
}

void requestApiKeyToServer() {
  Serial3.println("API_KEY 0");
}

void sendApiKeyToEsp(String apiKey) {
  String apiKeyCmd = "API_KEY " + (String)apiKey;
  Serial3.println(apiKeyCmd);
  Serial.println(apiKeyCmd);
}

String saveApiKeyServerResponse(String espResponse) {
  
    uint8_t firstKeyCharInd = espResponse.indexOf(":") + 1;
    String apiKey = espResponse.substring(firstKeyCharInd);
    File file = SD.open(apiKeyFilePath, FILE_WRITE);
    if (file) {
      file.print(apiKey);
      file.close();
      Serial.println("API key saved successfuly on SD card");
    } else {
      Serial.println("ERROR creating API key file");
    }
  return apiKey;
}

void printEspResponse(String espResponse){
  if (espResponse.length() > 0) {
    Serial.print("Response from ESP8266: ");
    Serial.println(espResponse);
  }
}
