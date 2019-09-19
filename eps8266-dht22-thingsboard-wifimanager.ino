#include <FS.h>

#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ThingsBoard.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

// DHT
#define DHTPIN 14
#define DHTTYPE DHT22
#define JSON_BUFFER 512

struct StoredConfig {
  char ssid[32];
  char password[64];
  char token[32];
  boolean success;
};

struct SensorData {
  float temperature;
  float humidity;
  boolean success;
};

char thingsboardServer[] = "demo.thingsboard.io";

WiFiClient wifiClient;

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

ThingsBoard tb(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

const int buttonPin = 12;
boolean isConfigMode;

StoredConfig storedConfig;

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void readMode() {
  // read the state of the pushbutton value:
  int buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    Serial.println("Mode is config");
    isConfigMode = true;
  } else {
    Serial.println("Mode is normal");
    isConfigMode = false;
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  readMode();

  //clean FS, for testing
  //SPIFFS.format();

  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading

      printFile();

      storedConfig = readConfig();
    } else {
      Serial.println("No config file");
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read

  if (isConfigMode) {

    openWifiManager();

  } else {
    dht.begin();
    delay(10);
    InitWiFi();
    lastSend = 0;

  }
}

void loop()
{
  if (isConfigMode) {
    return;
  }
  if (!storedConfig.success) {
    return;
  }

  if ( !tb.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds

    SensorData sd = readSensorData();
    if (sd.success) {
      tb.sendTelemetryFloat("temperature", sd.temperature);
      tb.sendTelemetryFloat("humidity", sd.humidity);
    }

    lastSend = millis();
  }

  tb.loop();
}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(storedConfig.ssid, storedConfig.password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}

void reconnect() {
  // Loop until we're reconnected
  while (!tb.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(storedConfig.ssid, storedConfig.password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    if ( tb.connect(thingsboardServer, storedConfig.token) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED]" );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}
