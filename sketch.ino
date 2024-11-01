#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include "bitmaps.h"
#include <SD.h>
#include "sd-utilities.h"
#include "audio-utilities.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include "http-utilities.h"
#include <ArduinoJson.h>

#define DEVICE_UUID "9479d017-d916-486d-8644-ee178a3d60f4"
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6
#define API_BASE_URL "https://guardianarea.azurewebsites.net/api/v1"
#define VP 39
#define VN 36
#define BPM_SCL_PIN VN
#define SPO2_SDA_PIN VP
#define SPO2_TRESHOLD_IRD_PIN 26
#define BPM_TRESHOLD_INT_PIN 25
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define PLAYBACK_BUTTON_PIN 6
#define SPEAKER_PIN 23
#define SD_CS_PIN 27

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
String tokenStr;
StaticJsonDocument<200> doc;
uint16_t frame = 0;

void setup() {
  Serial.begin(115200);

  //Pin Configuration
  pinMode(BPM_SCL_PIN, INPUT);
  pinMode(SPO2_SDA_PIN, INPUT);
  pinMode(SPO2_TRESHOLD_IRD_PIN, INPUT);
  pinMode(BPM_TRESHOLD_INT_PIN, INPUT);
  pinMode(RECORD_BUTTON_PIN, INPUT_PULLUP);
  pinMode(SPEAKER_PIN, OUTPUT);

  //OLED Screen
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("failed to start SSD1306 OLED"));
    while (1);
  }
  delay(2000);
  //OLED GuardianArea Logo
  oled.clearDisplay();
  oled.drawBitmap(0, 0, guardianAreaLogo, SCREEN_WIDTH, SCREEN_HEIGHT-10, WHITE);
  oled.setCursor(28,55);
  oled.setTextColor(WHITE);
  oled.println("GuardianArea");
  oled.display();
  delay(2000);

  //OLED Wi-Fi Initialization Animation
  oled.clearDisplay();

  //Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  while (WiFi.status() != WL_CONNECTED) {
    oled.drawBitmap(48, 5, internetFrames[frame], HEART_FRAME_WIDTH, HEART_FRAME_HEIGHT, 1);
    frame = (frame + 1) % HEART_FRAME_COUNT;
    oled.setCursor(0,HEART_FRAME_HEIGHT + 10);
    oled.print("Establishing Internet Connection");
    oled.display();
  }

  //SD Card
  Serial.print("Initializing SD card...");
  SD.begin(SD_CS_PIN);
  Serial.println("DONE.");
  Serial.println("Listing root dir...");
  File root = SD.open("/");
  printDirectory(root, 0);
  root.close();

  //GuardianArea API Token 
  File token = SD.open("/token.txt", FILE_READ);
  tokenStr = "";
  //If token.txt exists and can be opened
  if(token){
    //If token.txt is not empty then read the content
    if(token.available()){
      while (token.available()) {
          tokenStr += (char)token.read();
      }
      token.close();
    }
    //token.txt is empty, request it to the server
    else{
      token.close();
      token = SD.open("/token.txt", FILE_WRITE);
      String jsonUUID;
      doc["guardianAreaDeviceRecordId"] = DEVICE_UUID;
      serializeJson(doc, jsonUUID);
      String registerUrlStr = String(API_BASE_URL) + "/devices/register";
      String jsonToken = post(registerUrlStr.c_str(), jsonUUID.c_str());
      doc.clear();
      DeserializationError error = deserializeJson(doc, jsonToken);
      if (error) {
        Serial.print("Error al analizar JSON: ");
        Serial.println(error.c_str());
      }
      tokenStr = String(doc["apiKey"]);
      Serial.print("Token successfuly obtained: ");
      Serial.println(tokenStr);
      token.print(tokenStr);
      doc.clear();
      token.close();
    }
  }
  else{
    Serial.println("An error has ocurred when opening the token.txt file");
  }
}

void loop() {
  char buffer [10];
  uint16_t bpm = analogRead(BPM_SCL_PIN);
  uint16_t spo2 = analogRead(SPO2_SDA_PIN);
  bpm = map(bpm, 0, 4095, 0, 300);
  spo2 = map(spo2, 0, 4095, 0, 100);

  oled.clearDisplay();
  //BPM
  oled.drawBitmap(FRAME_OFFSET_X, FRAME_OFFSET_Y, heartFrames[frame], HEART_FRAME_WIDTH, HEART_FRAME_HEIGHT, 1);
  frame = (frame + 1) % HEART_FRAME_COUNT;
  oled.setCursor(FRAME_OFFSET_X + HEART_FRAME_WIDTH + 5,HEART_FRAME_HEIGHT/3+FRAME_OFFSET_Y);
  oled.print("BPM: ");
  oled.print(bpm);
  if(digitalRead(BPM_TRESHOLD_INT_PIN) == HIGH){
    oled.print("  X");
  }
  //SpO2 
  oled.setCursor(FRAME_OFFSET_X + HEART_FRAME_WIDTH + 5,HEART_FRAME_HEIGHT/3+10+FRAME_OFFSET_Y);
  oled.print("SpO2: ");
  oled.print(spo2);
  if(digitalRead(SPO2_TRESHOLD_IRD_PIN) == LOW){
    oled.print("  X");
  }
  oled.display();

  if (digitalRead(RECORD_BUTTON_PIN) == LOW) {
    recordAudio();
    post("https://guardianarea.azurewebsites.net/api/v1/authentication/sign-up","{\"username\": \"test4\",\"email\": \"test@test.com\",\"firstName\": \"test\",\"lastName\": \"test\",\"password\": \"test123\",\"roles\": [\"ROLE_ADMIN\"]}");
  }

}