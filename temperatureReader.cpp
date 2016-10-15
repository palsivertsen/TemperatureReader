#include <Arduino.h>
#include <OneWire.h>
#include "temperatureReader.h"

TemperatureReader::TemperatureReader(uint8_t pin)
    :numDevices(0),
    romSpace(0),
    deviceRoms(NULL) {
  oneWire = new OneWire(pin);
  pinMode(pin, INPUT_PULLUP);
}

void TemperatureReader::searchDevices() {
  numDevices = 0;
  debug_tr_println("Searching for devices");
  uint8_t rom[8];
  while (oneWire->search(rom)) {
    if (oneWire->crc8(rom, 7) != rom[7]) {
      debug_tr_println("Invalid CRC!");
      continue;
    }
    numDevices++;
    if (numDevices > romSpace) {
      debug_tr_print("Increasing deviceRoms to size: ");
      debug_tr_println(numDevices);
      deviceRoms = (uint8_t*)realloc(deviceRoms, numDevices * 8);
      if (deviceRoms == NULL) {
        debug_tr_println("Reallocation failed");
      }
      romSpace = numDevices;
    }
    debug_tr_print("Found:");
    for (int i = (numDevices * 8) - 8; i < numDevices * 8; i++) {
      deviceRoms[i] = rom[i%8];
      debug_tr_write(' ');
      debug_tr_print(deviceRoms[i], HEX);
    }
    debug_tr_println();
  }
  if (romSpace > numDevices) {
    debug_tr_print("Decreasing deviceRoms to size: ");
    debug_tr_println(numDevices);
    deviceRoms = (uint8_t*)realloc(deviceRoms, numDevices * 8);
    if (deviceRoms == NULL) {
      debug_tr_println("Reallocation failed");
    }
    romSpace = numDevices;
  }
  oneWire->reset_search();
}

int TemperatureReader::getNumDevices() {
  return numDevices;
}

int TemperatureReader::deviceTemperature(int id, float *temperature) {
  // Convert temperature
  reset();
  oneWire->select(&deviceRoms[id*8]);
  oneWire->write(0x44);
  // Wait for convertion
  do {
    oneWire->write_bit(1);
  } while (!oneWire->read_bit());

  reset();
  oneWire->select(&deviceRoms[id*8]);
  oneWire->write(0xBE);
  debug_tr_print("Data:");
  uint8_t data[9];
  for (int i = 0; i < 9; i++) {
    data[i] = oneWire->read();
    debug_tr_write(' ');
    debug_tr_print(data[i], HEX);
  }
  uint8_t crc = OneWire::crc8(data, 8);
  if (crc != data[8]) {
    debug_tr_println("Invalid CRC in scratchpad. Skipping.");
    return 1;
  }
  debug_tr_print(" CRC=");
  debug_tr_print(crc, HEX);
  debug_tr_println();
  int16_t raw = (data[1] << 8) | data[0];
  byte cfg = (data[4] & 0x60);
  // at lower res, the low bits are undefined, so let's zero them
  if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
  else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
  else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  // default is 12 bit resolution, 750 ms conversion time
  *temperature = (float)raw / 16.0f;
  debug_tr_print("Temperature is: ");
  debug_tr_println(*temperature, 4);
  return 0;
}

void TemperatureReader::deviceId(int index, char (*result)[19]) {
  for (int i = 0; i < 8; i++) {
    sprintf(&(*result)[i*2], "%02X", deviceRoms[index*8+i]);
  }
}

void TemperatureReader::reset() {
  if (oneWire->reset()) {
    debug_tr_println("Devices are responding");
  } else {
    debug_tr_println("No devices available");
  }
}
