#GuardianArea IoT Monitoring Bracelet Device

##Summary
Embedded Application for Guardian Area Monitoring Bracelet devices made by Digital Dart. The device uses several sensors and modules like GPS, pulsioximeter, microphone and OLED screen to help caretakers in monitoring children or elderly people. The C++ Sketch Application connects to the internet to send the collected sensors data to DigitalDart's Web Services for the necesary data processing. Once processed, the data is shown in a user-friendly manner to the user via GuardianArea's Mobile App and Web applications.

#Components
- ESP32
- OLED 1306 I2C Display
- MAX30102 Pulse Sensor module (Custom Chip)
- MAX9814 Microphone module
- Speaker
- SD Card Reader module
- SD Card
- ESP8266 ESP-01 Serial Wi-Fi module
- SIM800L GSM Module
- GY-NEO6MV2 GPS Module with antenna
- ADXL345 Accelerometer module
- Voltage Divider
- 10K Ohm and 20K Ohm Resistors
- DC Jack Module
- AC to DC Power Adapter
- 830 points protoboard

#Libraries
- Wire
- Adafruit_GFX
- Adafruit_SSD1306
- SPI
- SD
- WiFi
- WiFiClient
- HTTPClient
- ArduinoJson
- Wokwi Custom Chip API