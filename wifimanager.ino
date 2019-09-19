void openWifiManager() {
  Serial.println("Open wifimanager");
  //TODO
  Serial.println(storedConfig.ssid);
  Serial.println(storedConfig.password);
  Serial.println(storedConfig.token);

  WiFiManagerParameter token_param("token", "Token", storedConfig.token, TOKEN_SIZE);

  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  wifiManager.addParameter(&token_param);

  //reset settings - for testing
  //wifiManager.resetSettings();

  if (!wifiManager.startConfigPortal()) {
    //TODO timeout??
    Serial.println("failed to connect and hit timeout");
    //delay(3000);
    //reset and try again, or maybe put it to deep sleep
    //ESP.reset();
    //delay(5000);
    return;
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    StoredConfig c;
    strcpy(c.ssid, WiFi.SSID().c_str());
    strcpy(c.password, WiFi.psk().c_str());
    strcpy(c.token, token_param.getValue());

    saveConfig(c);
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());

}
