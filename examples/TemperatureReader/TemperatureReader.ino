#include <temperatureReader.h>

TemperatureReader temperatureReader(2);

void setup() {
  Serial.begin(9600);
  temperatureReader.searchDevices();
  Serial.print("Found ");
  Serial.print(temperatureReader.getNumDevices());
  Serial.println(" devices");
  Serial.println("Setup complete");
}

void loop() {
  for (int deviceIndex = 0; deviceIndex < temperatureReader.getNumDevices(); deviceIndex++) {
    float temperature;
    char id[19];
    if (temperatureReader.deviceTemperature(deviceIndex, &temperature)) {
      Serial.println("Checksum mismatch. Skipping");
      return;
    }
    temperatureReader.deviceId(deviceIndex, &id);
    Serial.print("Device id:\t");
    Serial.println(id);
    Serial.print("Temperature:\t");
    Serial.println(temperature);
    Serial.println();

  }
  delay(1000);
}
