void printFile() {
  Serial.println("Config file content:");
  File file = SPIFFS.open(CONFIG_FILE, "r");
  if (!file) {
    Serial.println("Failed to open config file");
    return;
  }
  while (file.available()) {
    Serial.print((char)file.read());
  }
  file.close();
}

StoredConfig readConfig() {

  printFile();

  StoredConfig c;
  c.ssid[0] = '\0';
  c.password[0] = '\0';
  c.token[0] = '\0';

  Serial.println("Reading config file");
  File configFile = SPIFFS.open(CONFIG_FILE, "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    c.success = false;
    return c;
  }
  Serial.println("Opened config file");

  StaticJsonDocument<JSON_BUFFER_SIZE> doc;

  Serial.println("Deserializing config file");
  DeserializationError error = deserializeJson(doc, configFile);
  if (error) {
    Serial.print("Deserialize json failed: ");
    Serial.println(error.c_str());
    c.success = false;
    return c;
  }

  configFile.close();

  Serial.println("Parsed json");

  JsonVariant ssid = doc[JSON_SSID];
  JsonVariant password = doc[JSON_PASSWORD];
  JsonVariant token = doc[JSON_TOKEN];
  if (ssid.isNull() || password.isNull() || token.isNull()) {
    Serial.println("Invalid json");
    c.success = false;
    return c;
  }

  strcpy(c.ssid, ssid.as<char*>());
  strcpy(c.password, password.as<char*>());
  strcpy(c.token, token.as<char*>());
  c.success = true;

  Serial.println("Config values:");
  Serial.print("WiFi SSID: ");
  Serial.println(c.ssid);
  Serial.print("WiFi password: ");
  Serial.println(c.password);
  Serial.print("Token: ");
  Serial.println(c.token);

  return c;
}

void saveConfig(StoredConfig c) {
  Serial.println("Saving config");

  StaticJsonDocument<JSON_BUFFER_SIZE> doc;

  doc[JSON_SSID] = c.ssid;
  doc[JSON_PASSWORD] = c.password;
  doc[JSON_TOKEN] = c.token;

  File configFile = SPIFFS.open(CONFIG_FILE, "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
  } else {
    // Serialize JSON to file
    if (serializeJsonPretty(doc, configFile) == 0) {
      Serial.println("Failed to write to file");
    }
    configFile.close();

    printFile();
  }

}
