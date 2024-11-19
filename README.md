# GuardianArea IoT Monitoring Bracelet Device

## Summary
Embedded Application for Guardian Area Monitoring Bracelet devices made by Digital Dart. The device uses several sensors and modules like GPS, pulsioximeter, OLED screen, accelerometer, Wi-Fi and SD Card reader. Created to help caretakers in monitoring children or elderly people. The C++ Sketch Application connects to the internet to send via websockets the collected sensors data to DigitalDart's Web Services for the necesary data processing. Once processed, the data is shown in a user-friendly manner to the user via GuardianArea's Mobile App and Web applications.

# Components
- Arduino MEGA
- ESP8266 ESP01 Serial Wi-Fi Module
- OLED 1306 Monochrome I2C Display
- MAX30102 Pulse Sensor module
- MAX9814 Microphone module
- Speaker
- SD Card Reader module
- 4GB SD Card
- SIM800L GSM Module
- GY-NEO6MV2 GPS Module with antenna
- ADXL345 Accelerometer module
- Voltage Divider
- 10K Ohm and 20K Ohm Resistors
- DC Jack Module
- AC to DC Power Adapter
- 830 points protoboard

# Libraries
- SparkFun_MAX3010x_Pulse_and_Proximity_Sensor_Library
- ArduinoJson
- Adafruit_ADXL345_U
- U8g2lib
- Wire
- SD
- SPI
- TinyGPS++
- ESP8266WiFi
- ESP8266HTTPClient
- WebSocketsClient
