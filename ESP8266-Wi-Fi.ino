#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

const char* ssid = "HUGO";
const char* password = "1820222426";
const char* httpRegisterPath = "/api/v1/devices/register";
const char* baseUrl = "guardianarea.azurewebsites.net";
const char* wsHealthPath = "/health-measures-stream?room=";
const char* wsLocationPath = "/location-stream?room=";
const uint16_t wsUnsafePort = 80;

String deviceUUID = "";
String apiKey = "";
WiFiClient client;
WebSocketsClient healthWebSocket;
WebSocketsClient locationWebSocket;
WebSocketsClient accelerometerWebSocket;
bool wsInitialized = false;

void connectToWiFi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WI-FI CONNECTED");
}

String performGET(const String& url) {
  HTTPClient http;
  http.begin(client, url);
  int httpCode = http.GET();
  String payload = "";
  
  if (httpCode > 0) {
    payload = http.getString();
  } else {
    payload = "Error in GET request";
  }
  
  http.end();
  return payload;
}

String performPOST(const String& url, const String& payload) {
  HTTPClient http;
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(payload);
  String response = "";
  
  if (httpCode > 0) {
    response = http.getString();
  } else {
    response = "Error in POST request";
  }
  
  http.end();
  return response;
}

void getApiKey(){
  String registerUrl = "http://" + (String) baseUrl + (String) httpRegisterPath;
  JsonDocument doc;
  doc["guardianAreaDeviceRecordId"] = deviceUUID;
  String payload;
  serializeJson(doc, payload);
  Serial.print("Registering device on: ");
  Serial.println(registerUrl);
  Serial.print("Sending payload: ");
  Serial.println(payload);
  String response = performPOST(registerUrl, payload);
  doc.clear();
  deserializeJson(doc, response);
  apiKey = (String) doc["apiKey"];
  String apiKeyCmd = "API_KEY:" + apiKey;
  Serial.println(apiKeyCmd);
}

void processCommand(String command) {
  String key = ""; 
  String firstParameter = "";
  String secondParameter = "";

  int firstSpace = command.indexOf(" ");
  int secondSpace = command.indexOf(" ", firstSpace + 1);

  if (firstSpace != -1) {
    key = command.substring(0, firstSpace); //'key' is the METHOD in HTTP requests or API_KEY key name

    if (secondSpace != -1) {
      firstParameter = command.substring(firstSpace + 1, secondSpace);  //'firstParameter' is the URL in HTTP requests
      secondParameter = command.substring(secondSpace + 1);             //'secondParamter' is the JSON BODY in HTTP requests
    } else {
      firstParameter = command.substring(firstSpace + 1);
      firstParameter.trim(); 
    }
  } else {
    Serial.println("Invalid command");
    return;
  }

  String response = "";
  if(key == "API_KEY"){
    apiKey = firstParameter;
    if(!apiKey.equals("0")){
      response = "Guardian Area API key received, will be used in future requests.";
    }
    else{
      response = "Signing up device. Obtaining API Key...";
    }
  }
  else if(key == "DEVICE_ID"){
    deviceUUID = firstParameter;
  }
  else if(key == "GET"){
    response = performGET(firstParameter);
  }
  else if(key == "POST"){
    response = performPOST(firstParameter, secondParameter);
  }
  else if(key == "WS_HEALTH"){
    healthWebSocket.sendTXT(firstParameter);
  }
  else if(key == "WS_LOCATION"){
    locationWebSocket.sendTXT(firstParameter);
  }
  else if(key == "WS_ACCELEROMETER"){
    accelerometerWebSocket.sendTXT(firstParameter);
  }
  else{
    response = "Method not supported";
  }
  
  Serial.println(response);
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket connected");
      break;
    case WStype_TEXT:
      Serial.printf("Received text: %s\n", payload);
      break;
    case WStype_BIN:
      Serial.printf("Received binary data of length %d\n", length);
      break;
    default:
      break;
  }
}

void initWebSockets(){
  String wsHealthUrl = (String)baseUrl + (String)wsHealthPath + apiKey;
  String wsHealthCompletePath = (String)wsHealthPath + apiKey;
  Serial.print("WEBSOCKETS-HEALTH: Connecting to ");
  Serial.println(wsHealthUrl);
  healthWebSocket.begin(baseUrl, wsUnsafePort, wsHealthCompletePath.c_str());
  healthWebSocket.onEvent(webSocketEvent);
  healthWebSocket.setReconnectInterval(5000);

  String wsLocationUrl = (String)baseUrl + (String)wsLocationPath + apiKey;
  String wsLocationCompletePath = (String)wsLocationPath + apiKey;
  Serial.print("WEBSOCKETS-LOCATION: Connecting to ");
  Serial.println(wsLocationUrl);
  locationWebSocket.begin(baseUrl, wsUnsafePort, wsLocationCompletePath.c_str());
  locationWebSocket.onEvent(webSocketEvent);
  locationWebSocket.setReconnectInterval(5000);

  String wsAccelerometerUrl = (String)baseUrl + (String)wsAccelerometerPath + apiKey;
  String wsAccelerometerCompletePath = (String)wsAccelerometerPath + apiKey;
  Serial.print("WEBSOCKETS-ACCELEROMETER: Connecting to ");
  Serial.println(wsAccelerometerUrl);
  accelerometerWebSocket.begin(baseUrl, wsUnsafePort, wsAccelerometerCompletePath.c_str());
  accelerometerWebSocket.onEvent(webSocketEvent);
  accelerometerWebSocket.setReconnectInterval(5000);

  Serial.println("WebSocket connection initialized");
}


void setup() {
  Serial.begin(9600);
  //Serial.setDebugOutput(true);
  connectToWiFi();
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    processCommand(command);
  }

  if (!apiKey.isEmpty() && !apiKey.equals("0") && !wsInitialized && WiFi.status() == WL_CONNECTED) {
    initWebSockets();
    wsInitialized = true;
  }

  if(apiKey.equals("0") && !deviceUUID.isEmpty() && WiFi.status() == WL_CONNECTED){
    getApiKey();
  }
  
  healthWebSocket.loop();
  locationWebSocket.loop();
}
