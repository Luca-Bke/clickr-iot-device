#include <FS.h>
#include <ESP8266WiFi.h>
#include <ThingsBoard.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <Adafruit_MCP23X17.h>



#define MCP23017_ADRESS 0x20
Adafruit_MCP23X17 mcp;



//memory pool size for JsonDocument
#define JSON_BUFFER_SIZE 1024
//for safety
#define MAX_FILE_SIZE 512
//json field names
#define JSON_SSID "ssid"
#define JSON_PASSWORD "password"
#define JSON_SERVER "server"
#define JSON_PORT "port"
#define JSON_TOKEN "token"
//max sizes of paramaters in config
#define SSID_SIZE 32
#define PASSWORD_SIZE 64
#define SERVER_SIZE 64
#define PORT_SIZE 64
#define TOKEN_SIZE 64
//filename for config
#define CONFIG_FILE "/config.json"
//default server to send data to
#define TB_SERVER_DEFAULT "192.168.1.7"
#define TB_PORT_DEFAULT "8080"

int ledPins[] = { 1, 2, 3, 5, 6, 7, 11, 10, 9, 15, 14, 13};
int ledState[] = { 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1};
int btnPins[] = { 0, 4, 8, 12};
boolean btnState[] = {false, false, false, false};

struct StoredConfig {
  char ssid[SSID_SIZE];
  char password[PASSWORD_SIZE];
  char server[SERVER_SIZE];
  char port[PORT_SIZE];
  char token[TOKEN_SIZE];
  boolean success = false;
};

struct SensorData {
  float temperature;
  float humidity;
  boolean success = false;
};

WiFiClient wifiClient;

PubSubClient ps(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

boolean isConfigMode = false;

StoredConfig storedConfig;

void readMode() {
  for (int i = 0; i < sizeof(btnPins) / sizeof(btnPins[0]); i++) {
    if (mcp.digitalRead(btnPins[i]) == 1) { isConfigMode = true; }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  mcp.begin_I2C(MCP23017_ADRESS);
  // initialize the pushbutton pin as an input:
  Serial.print("Setting Pin to LED Pin:");
  for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++) {
    Serial.print(ledPins[i]);
    Serial.print(",");
    mcp.pinMode(ledPins[i], OUTPUT);
    mcp.digitalWrite(ledPins[i], HIGH);
  }
  Serial.println("");

  updateLED();


  Serial.print("Setting Pin to Button Pin:");
  for (int i = 0; i < sizeof(btnPins) / sizeof(btnPins[0]); i++) {
    Serial.print(btnPins[i]);
    Serial.print(",");
    mcp.pinMode(btnPins[i], INPUT);
  }
  Serial.println("");


  readMode();

  //clean FS, for testing
  // SPIFFS.format();

  initConfig(&storedConfig);
  
  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists(CONFIG_FILE)) {
      //file exists, reading and loading
      storedConfig = readConfig();
    } else {
      Serial.println("No config file");
    }
  } else {
    Serial.println("failed to mount FS");
  }

  if (isConfigMode) {
    openWifiManager();
    return;
  }

  if (!storedConfig.success) {
    return;
  }

  delay(10);
  lastSend = 0;

  connectAndWaitWifi();
}

void loop()
{
  if (isConfigMode) {
    return;
  }
  if (!storedConfig.success) {
    return;
  }

  if ( !ps.connected() ) {
    reconnect();
  }



  for (int i = 0; i < sizeof(btnPins) / sizeof(btnPins[0]); i++) {
    if (mcp.digitalRead(btnPins[i]) != btnState[i]) {
      Serial.print("Btn: ");
      Serial.print(i);
      Serial.print(" State: ");
      Serial.print(mcp.digitalRead(btnPins[i]));
      btnState[i] = mcp.digitalRead(btnPins[i]);
      Serial.println();


      if(btnState[i] == 1){
        DynamicJsonDocument mdoc(2048);
        mdoc["btn"] = i;
        mdoc["eventtype"] = "pressed";
        char payload[128];
        int b =serializeJson(mdoc, payload);
        ps.publish(String("v1/devices/me/telemetry").c_str(), payload, true);
      }
    }
  }




  ps.loop();
}

void reconnect() {
  Serial.println("Connecting to ThingsBoard MQTT");
  while (!ps.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      connectAndWaitWifi();
    }
    Serial.print("Connecting to ThingsBoard MQTT ...");

    
    IPAddress ip;

    ip.fromString(storedConfig.server);
    ps.setServer(ip, 1883);
    ps.setCallback(mqttCallback);

    if (ps.connect(String("Clickr_Device").c_str(), storedConfig.token, String("").c_str())) {
      Serial.println( "[DONE]" );

      ps.subscribe("v1/devices/me/attributes");
      ps.publish("v1/devices/me/attributes/request/2","{'sharedKeys':'config'}");
    } else {
      Serial.print( "[FAILED]" );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length){
  Serial.println("MQTT Recived");
  StaticJsonDocument<512> doc;
  deserializeJson(doc, payload, length);
  // char buffer[2000];
  // serializeJsonPretty(doc, buffer);
  // Serial.println(buffer);
  JsonObject jsonConfig;
  if(doc.containsKey("config")){
    Serial.println("Recieved Config from Server");
    jsonConfig = doc["config"].as<JsonObject>();
  }
  else if(doc.containsKey("shared")){
    Serial.println("Recieved requested Config from Server");
    jsonConfig = doc["shared"]["config"].as<JsonObject>();
  }
  
  if(!jsonConfig.isNull()){
    Serial.println("Config Found");
    for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++) {
      int led_color = i % 3;
      int led = i/3;
      // Serial.print("LED: ");
      // Serial.print(led);
      // Serial.print(" Farbe: ");
      // Serial.print(led_color);
      // Serial.println("");
      switch (led_color) {
        case 0:
          ledState[i] = jsonConfig["buttons"][led]["colors"]["red"].as<int>();
          break;
        case 1:
          ledState[i] = jsonConfig["buttons"][led]["colors"]["green"].as<int>();
          break;
        case 2:
          ledState[i] = jsonConfig["buttons"][led]["colors"]["blue"].as<int>();
          break;
        default:
          break;
      }
      // Serial.print("State: ");
      // Serial.println(ledState[i]);
    }
  }
  ps.loop();
  updateLED();
}
void updateLED(){
  Serial.print("Updating LED ");
  for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++) {
    // Serial.println(i);
    mcp.digitalWrite(ledPins[i],ledState[i]);
  }
}
