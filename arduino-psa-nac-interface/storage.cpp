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
  if (storage.version != 4) {
    storage = { 4, { 0, 0, 0, 1, 1, 2021}, { 0, 0, 0, 1, 1, 2020}, 6, C, km, VolFor100Units, l};
    saveToStorage();
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

DateTime StorageClass::getStartDateTime(){
  return storage.startDateTime;
}

byte StorageClass::getLanguageId() {
  return storage.languageId;
}
void StorageClass::setLanguageId(byte value) {
  storage.languageId = value;
  saveToStorage();
}

TemperatureUnitValues StorageClass::getTemperatureUnit() {
  return storage.temperatureUnit;
}
void StorageClass::setTemperatureUnit(TemperatureUnitValues value) {
  storage.temperatureUnit = value;
  saveToStorage();
}

DistanceUnitValues StorageClass::getDistanceUnit() {
  return storage.distanceUnit;
}
void StorageClass::setDistanceUnit(DistanceUnitValues value) {
  storage.distanceUnit = value;
  saveToStorage();
}

ConsumptionUnitValues StorageClass::getConsumptionUnit() {
  return storage.consumptionUnit;
}
void StorageClass::setConsumptionUnit(ConsumptionUnitValues  value) {
  storage.consumptionUnit = value;
  saveToStorage();
}

VolumeUnitValues StorageClass::getVolumeUnit() {
  return storage.volumeUnit;
}
void StorageClass::setVolumeUnit(VolumeUnitValues value) {
  storage.volumeUnit = value;
  saveToStorage();
}

StorageClass Storage;
