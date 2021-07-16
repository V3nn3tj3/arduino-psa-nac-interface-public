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
#include "configuration.h"

ConfigurationClass::ConfigurationClass() {
}

char ConfigurationClass::getVINNumberChar(byte index) {
  return VINNumber[index];
}

byte ConfigurationClass::getLanguageAndUnit() {
  return Storage.getLanguageAndUnit();
}
void ConfigurationClass::setLanguageAndUnit(byte value) {
  Storage.setLanguageAndUnit(value);
}

byte ConfigurationClass::getLanguageId() {
  return Storage.getLanguageId();
}
void ConfigurationClass::setLanguageId(byte value) {
  Storage.setLanguageId(value);
}

TemperatureReadingValues ConfigurationClass::getTemperatureReading() {
  return Storage.getTemperatureReading();
}
void ConfigurationClass::setTemperatureReading(TemperatureReadingValues value) {
  Storage.setTemperatureReading(value);
}

FuelStatValues ConfigurationClass::getFuelStat() {
  return Storage.getFuelStat();
}
void ConfigurationClass::setFuelStat(FuelStatValues value) {
  Storage.setFuelStat(value);
}

DateTime ConfigurationClass::getDateTime() {
  return Storage.getDateTime();
}
void ConfigurationClass::setDateTime(DateTime value) {
  Storage.setDateTime(value);
}

ConfigurationClass Configuration;
