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

StoredConfig readConfig() {
  StoredConfig c;
  c.ssid[0] = '\0';
  c.password[0] = '\0';
  c.token[0] = '\0';

  Serial.println("Reading config file");
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    c.success = false;
    return c;
  }
  Serial.println("Opened config file");

  StaticJsonDocument<JSON_BUFFER> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, configFile);
  if (error) {
    Serial.print("Deserialize json failed: ");
    Serial.println(error.c_str());
    c.success = false;
    return c;
  }

  configFile.close();

  Serial.println("Parsed json");

  JsonVariant ssid = doc["ssid"];
  JsonVariant password = doc["password"];
  JsonVariant token = doc["token"];
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
  Serial.print("Wifi SSID: ");
  Serial.println(c.ssid);
  Serial.print("Wifi password: ");
  Serial.println(c.password);
  Serial.print("Token: ");
  Serial.println(c.token);

  return c;
}

void saveConfig(char * token) {
  Serial.println("saving config");

  StaticJsonDocument<JSON_BUFFER> doc;

  doc["ssid"] = WiFi.SSID();
  doc["password"] = WiFi.psk();
  doc["token"] = token;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  } else {
    // Serialize JSON to file
    if (serializeJson(doc, configFile) == 0) {
      Serial.println("Failed to write to file");
    }
    configFile.close();

    printFile();
  }

}
