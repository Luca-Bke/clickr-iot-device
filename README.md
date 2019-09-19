# eps8266-dht22-thingsboard-wifimanager
Send temperature and humidity from DHT22 sensor to ThingsBoard IoT Platform.

Wifi ssid/password and ThingsBoard token are configured in runtime through Web Browser, settings are stored permanently in ESP8266 flash (SPIFFS).

## Usage
1. Register in ThingsBoard (https://thingsboard.io/) and get token for device
1. Before uploading sketch change flash settings, choose minimum SPIFSS value, for example 512k/32k
1. Upload sketch
1. Switch button ON (enable config mode) and reset device (or power off and then on), connect to appeared wifi access point, save wifi settings and ThingsBoard token
1. Switch button OFF (enable normal mode), reset device  (or power off and then on), sensor data should appear in ThingsBoard dashboard
1. In case of no success open serial monitor for troubleshooting
1. If wifi ssid/password or token changes repeat steps 4-5

## Hardware
* ESP8266
* DHT22 temperature/humidity sensor
* 10k resistor to connect DHT22 (pull up)
* Switch button
* 10k resistor to connect switch button (pull down)

## Used libraries
* https://github.com/adafruit/DHT-sensor-library
* https://github.com/bblanchon/ArduinoJson
* https://github.com/tzapu/WiFiManager

## Useful links
* [ESP8266 Filesystem (SPIFFS)](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html)
* [ThingsBoard guide for DHT22](https://thingsboard.io/docs/samples/esp8266/temperature/)
