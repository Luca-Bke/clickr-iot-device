# eps8266-dht22-thingsboard-wifimanager
Send temperature and humidity from DHT22 sensor to ThingsBoard IoT Platform.

Wifi ssid/password and ThingsBoard token are configured in runtime through Web Browser, settings are stored permanently in ESP8266 flash (SPIFFS).

## Hardware
* ESP8266
* DHT22 temperature/humidity sensor
* 10k resistor to connect DHT22 (pull up)
* Switch button
* 10k resistor to connect switch button (pull down)

## Useful links
* [ESP8266 Filesystem (SPIFFS)](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html)
* [ThingsBoard guide for DHT22](https://thingsboard.io/docs/samples/esp8266/temperature/)
