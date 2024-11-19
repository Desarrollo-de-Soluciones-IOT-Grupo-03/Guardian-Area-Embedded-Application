#include "JsonUtils.h"
#include <ArduinoJson.h>

String jsonify(int numKeys, ...){
  JsonDocument doc; 
  va_list args;
  va_start(args, numKeys); 

  for (int i = 0; i < numKeys; i++) {
    const char* key = va_arg(args, const char*);    
    const char* value = va_arg(args, const char*); 
    doc[key] = value;
  }

  va_end(args); 

  String payload;
  serializeJson(doc, payload);
  return payload;
}