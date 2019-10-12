void printFile() {
  Serial.println("Config file content:");
  File file = SPIFFS.open(CONFIG_FILE, "r");
  if (!file) {
    Serial.println("Failed to open config file");
    return;
  }
  Serial.print("File size: ");
  Serial.println(file.size());

  if (file.size() > MAX_FILE_SIZE) {
    Serial.print("Config file is too large: ");
    Serial.println(file.size());
  } else {
    int i = 0;
    while (i < MAX_FILE_SIZE && file.available()) {
      i++;
      Serial.print((char)file.read());
    }
    if (i == MAX_FILE_SIZE) {
      Serial.println("\nUnexpected error");
    }
  }
  file.close();
  Serial.println();
}

void initConfig(StoredConfig * c) {
  c->ssid[0] = '\0';
  c->password[0] = '\0';
  strcpy(c->server, TB_SERVER_DEFAULT);
  c->token[0] = '\0';
  c->success = false;
}

StoredConfig readConfig() {

  printFile();

  //for safety
  delay(500);

  StoredConfig c;
  initConfig(&c);

  Serial.println("Reading config file");
  File file = SPIFFS.open(CONFIG_FILE, "r");
  if (!file) {
    Serial.println("Failed to open config file");
    return c;
  }
  if (file.size() > MAX_FILE_SIZE) {
    Serial.print("Config file is too large: ");
    Serial.println(file.size());
    return c;
  }
  Serial.println("Opened config file");

  StaticJsonDocument<JSON_BUFFER_SIZE> doc;

  Serial.println("Deserializing config file");
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.print("Deserialize json failed: ");
    Serial.println(error.c_str());
    return c;
  }

  file.close();

  Serial.println("Parsed json");

  JsonVariant ssid = doc[JSON_SSID];
  JsonVariant password = doc[JSON_PASSWORD];
  JsonVariant server = doc[JSON_SERVER];
  JsonVariant token = doc[JSON_TOKEN];
  if (!ssid.isNull()) {
    strcpy(c.ssid, ssid.as<char*>());
  } else {
    Serial.println("Missing field: ssid");
  }
  if (!password.isNull()) {
    strcpy(c.password, password.as<char*>());
  } else {
    Serial.println("Missing field: password");
  }
  if (!server.isNull()) {
    strcpy(c.server, server.as<char*>());
  } else {
    Serial.println("Missing field: server");
  }
  if (!token.isNull()) {
    strcpy(c.token, token.as<char*>());
  } else {
    Serial.println("Missing field: token");
  }
  if (ssid.isNull() || password.isNull() || server.isNull() || token.isNull()) {
    Serial.println("Invalid json");
    return c;
  }

  c.success = true;

  Serial.println("Config values:");
  Serial.print("WiFi SSID: ");
  Serial.println(c.ssid);
  Serial.print("WiFi password: ");
  Serial.println(c.password);
  Serial.print("Server: ");
  Serial.println(c.server);
  Serial.print("Token: ");
  Serial.println(c.token);

  return c;
}

void saveConfig(StoredConfig c) {
  Serial.println("Saving config");

  StaticJsonDocument<JSON_BUFFER_SIZE> doc;

  doc[JSON_SSID] = c.ssid;
  doc[JSON_PASSWORD] = c.password;
  doc[JSON_SERVER] = c.server;
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

    //for safety
    delay(500);

    printFile();
  }

}
