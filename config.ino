// Prints the content of a file to the Serial
void printFile() {
  // Open file for reading
  Serial.println("---File start---");
  File file = SPIFFS.open("/config.json", "r");
  if (!file) {
    Serial.println(F("Failed to read file"));
    return;
  }

  // Extract each characters by one by one
  while (file.available()) {
    Serial.print((char)file.read());
  }
  // Close the file
  file.close();

  Serial.println("\n---File end---");
}

void readConfig() {
  Serial.println("Reading config file");
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");;
    return;
  }
  Serial.println("Opened config file");;
  size_t size = configFile.size();
  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<JSON_BUFFER> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, buf.get());

  if (!error) {
    Serial.println("\nparsed json");

    strcpy(mqtt_server, doc["mqtt_server"]);
    strcpy(mqtt_port, doc["mqtt_port"]);
    strcpy(blynk_token, doc["blynk_token"]);

    Serial.println("Config values:");
    Serial.print("Wifi SSID: ");
    Serial.println(mqtt_server);
    Serial.print("Wifi password: ");
    Serial.println(mqtt_port);
    Serial.print("Token: ");
    Serial.println(blynk_token);

  } else {
    Serial.println("failed to load json config");
  }
  configFile.close();

}

void saveConfig(char * token) {
  Serial.println("saving config");

  //read updated parameters
  strcpy(blynk_token, token);

  StaticJsonDocument<JSON_BUFFER> doc;

  doc["mqtt_server"] = WiFi.SSID();
  doc["mqtt_port"] = WiFi.psk();
  doc["blynk_token"] = blynk_token;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  } else {
    // Serialize JSON to file

    // Serialize JSON to file
    if (serializeJson(doc, configFile) == 0) {
      Serial.println(F("Failed to write to file"));
    }
    configFile.close();

    printFile();
  }

}
