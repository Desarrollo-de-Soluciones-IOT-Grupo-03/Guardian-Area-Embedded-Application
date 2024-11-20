#include "SDUtils.h"
#include "ESP8266Interface.h"
#include "Constants.h"
#include "JsonUtils.h"
#include <Wire.h>
#include "HeartRateSpo2Utils.h"
#include "AccelerometerUtils.h"
#include "DisplayUtils.h"
#include "GPSUtils.h"

MAX30105 particleSensor;
String apiKey = "";
bool espAvailable = false;
unsigned long lastBpmUpdateTime = 0;

void setup() {
  Serial.begin(9600);   // Main Hardware Serial
  Serial2.begin(9600);  // GPS Hardware Serial
  Serial3.begin(9600);  // ESP8266 Wi-Fi Hardware Serial

  //SD Card
  initSdCard();
  apiKey = readApiKeyFile();

  //BPM and SPO2 sensor initialization
  initParticleSensor(particleSensor);

  //Accelerometer
  initAccelerometer();

  //SSD1315 OLED Display
  initDisplay();
}

void loop() {
  sendDataFromMainSerialToEsp();          // Main Serial 
  String espResponse = getEspResponse();  // ESP Serial
  printEspResponse(espResponse);
  if(espResponse.equals("WI-FI CONNECTED")){
    espAvailable = true;
    sendDeviceID();
    if(apiKey.equals("")){
      requestApiKeyToServer();
    }
    else{
      sendApiKeyToEsp(apiKey);
    }
  }
  if(espAvailable){
    if (espResponse.startsWith("API_KEY:")) {
      apiKey = saveApiKeyServerResponse(espResponse);
    }
    sendGpsDataToServer();
    if(millis() - lastBpmUpdateTime > 500){
      lastBpmUpdateTime = millis();
      sendHealthDataToServer(particleSensor);
    } 
    sendAccelerometerDataToServer();
  }
}
