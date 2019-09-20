void connectAndWaitWifi() {
  Serial.println("Connecting to AP");
  WiFi.begin(storedConfig.ssid, storedConfig.password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}
