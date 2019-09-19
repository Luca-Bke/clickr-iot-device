SensorData readSensorData()
{
  Serial.println("Collecting temperature data.");
  SensorData sd;
  sd.temperature = dht.readTemperature();
  sd.humidity = dht.readHumidity();

  if (isnan(sd.humidity) || isnan(sd.temperature)) {
    sd.success = false;
    Serial.println("Failed to read from DHT sensor!");
  } else {
    sd.success = true;
    Serial.print("Temperature: ");
    Serial.print(sd.temperature);
    Serial.print(", Humidity: ");
    Serial.println(sd.humidity);
  }

  return sd;
}
