#ifndef temperature_reader
#define temperature_reader
#include <Arduino.h>
#include <OneWire.h>

#ifdef DEBUG_TR
  #define debug_tr_print(...)    Serial.print(__VA_ARGS__)
  #define debug_tr_println(...)  Serial.println(__VA_ARGS__)
  #define debug_tr_write(...)    Serial.write(__VA_ARGS__)
#else
  #define debug_tr_print(...)
  #define debug_tr_println(...)
  #define debug_tr_write(...)
#endif

class TemperatureReader {
public:
  TemperatureReader(uint8_t);
  void searchDevices();
  int getNumDevices();
  int deviceTemperature(int, float*);
  void deviceId(int, char(*)[19]);
private:
  int numDevices;
  void reset();
  OneWire *oneWire;
  uint8_t *deviceRoms;
  int romSpace;
};
#endif
