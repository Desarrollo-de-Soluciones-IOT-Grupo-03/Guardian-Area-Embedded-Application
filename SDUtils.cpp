#include "SDUtils.h"
#include <SPI.h>
#include <SD.h>
#include "Constants.h"

void initSdCard(){
  if (!SD.begin()) {
    Serial.println("Error initializing SD card");
  } else {
    Serial.println("SD card initialized successfuly");
  }
}

String readApiKeyFile(){
  String apiKey = "";
  if (SD.exists(apiKeyFilePath)) {
    Serial.println("API key file exists, reading content...");
    File file = SD.open(apiKeyFilePath, FILE_READ);
    if (file) {
      while (file.available()) {
        apiKey += (char)file.read();
      }
      file.close();
      Serial.print("API key read from SD card: ");
      Serial.println(apiKey);
    } else {
      Serial.println("ERROR opening the file");
    }
  } else {
    Serial.println("API key file was not found. Requesting it to the server....");
  }
  return apiKey;
}