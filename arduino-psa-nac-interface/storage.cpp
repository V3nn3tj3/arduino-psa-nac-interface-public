/*
   Copyright 2021, Nick V. (V3nn3tj3) <https://github.com/v3nn3tj3>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "storage.h"
#include <EEPROM.h>

StorageClass::StorageClass() {
  loadFromStorage();
  if (storage.version != 1) {
    storage = { 1, { 0, 0, 0, 1, 1, 2021}, LanguageID, (LanguageID * 4) + 128, C, Lkm};
    saveToStorage();
  }
  if (!storage.languageAndUnit) {
    storage.languageAndUnit = (LanguageID * 4) + 0x80;
  }
}

void StorageClass::saveToStorage() {
  EEPROM.put(0, storage);
}

void StorageClass::loadFromStorage() {
  EEPROM.get(0, storage);
}

DateTime StorageClass::getDateTime() {
  return storage.dateTime;
}
void StorageClass::setDateTime(DateTime value) {
  storage.dateTime = value;
  saveToStorage();
}

byte StorageClass::getLanguageId() {
  return storage.languageId;
}
void StorageClass::setLanguageId(byte value) {
  storage.languageId = value;
  saveToStorage();
}

byte StorageClass::getLanguageAndUnit() {
  return storage.languageAndUnit;
}
void StorageClass::setLanguageAndUnit(byte value) {
  storage.languageAndUnit = value;
  saveToStorage();
}

TemperatureReadingValues StorageClass::getTemperatureReading() {
  return storage.temperatureReading;
}
void StorageClass::setTemperatureReading(TemperatureReadingValues value) {
  storage.temperatureReading = value;
  saveToStorage();
}

FuelStatValues StorageClass::getFuelStat() {
  return storage.fuelStat;
}
void StorageClass::setFuelStat(FuelStatValues value) {
  storage.fuelStat = value;
  saveToStorage();
}

StorageClass Storage;
