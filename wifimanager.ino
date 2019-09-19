void openWifiManager() {
  Serial.println("Open wifimanager");
  Serial.println(storedConfig.ssid);
  Serial.println(storedConfig.password);
  Serial.println(storedConfig.token);
  
  WiFiManagerParameter custom_blynk_token("token", "Token", storedConfig.token, 64);

  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  wifiManager.addParameter(&custom_blynk_token);

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
    saveConfig((char*) custom_blynk_token.getValue());
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());

}
