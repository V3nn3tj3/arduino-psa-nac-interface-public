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
#include "TimeLib.h"

ConfigurationClass::ConfigurationClass() {
}

char ConfigurationClass::getVINNumberChar(byte index) {
  return VINNumber[index];
}

byte ConfigurationClass::getLanguageId() {
  return Storage.getLanguageId();
}
void ConfigurationClass::setLanguageId(byte value) {
  Storage.setLanguageId(value);
}

bool ConfigurationClass::getFromLanguageBitOnPosition(byte position) {
  return (Storage.getLanguageId() >> position) & 0x1;
}

TemperatureUnitValues ConfigurationClass::getTemperatureUnit() {
  return Storage.getTemperatureUnit();
}
void ConfigurationClass::setTemperatureUnit(TemperatureUnitValues value) {
  Storage.setTemperatureUnit(value);
}

DistanceUnitValues ConfigurationClass::getDistanceUnit() {
  return Storage.getDistanceUnit();
}
void ConfigurationClass::setDistanceUnit(DistanceUnitValues value) {
  Storage.setDistanceUnit(value);
}

ConsumptionUnitValues ConfigurationClass::getConsumptionUnit() {
  return Storage.getConsumptionUnit();
}
void ConfigurationClass::setConsumptionUnit(ConsumptionUnitValues  value) {
  Storage.setConsumptionUnit(value);
}

VolumeUnitValues ConfigurationClass::getVolumeUnit() {
  return Storage.getVolumeUnit();
}
void ConfigurationClass::setVolumeUnit(VolumeUnitValues value) {
  Storage.setVolumeUnit(value);
}

DateTime ConfigurationClass::getDateTime() {
  return Storage.getDateTime();
}
void ConfigurationClass::setDateTime(DateTime value) {
  Storage.setDateTime(value);
}

int ConfigurationClass::getDayOfYear(){
  // Given a day, month, and year (4 digit), returns 
  // the day of year. Errors return 999.
  int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
  // Check if it is a leap year, this is confusing business
  // See: https://support.microsoft.com/en-us/kb/214019
  if (year()%4  == 0) {
    if (year()%100 != 0) {
      daysInMonth[1] = 29;
    }
    else {
      if (year()%400 == 0) {
        daysInMonth[1] = 29;
      }
    }
   }
  
  int doy = 0;
  for (int i = 0; i < month() - 1; i++) {
    doy += daysInMonth[i];
  }
  
  doy += day();
  return doy;
}

unsigned long ConfigurationClass::getSecondsFromDay(){
 return (long)hour() * (long)3600 + minute() * 60 + second();
}

int ConfigurationClass::getYearsRunning(){
  int yearsRunning = year() - Storage.getStartDateTime().Year;
  if(yearsRunning > 0){
    return yearsRunning;
  }
  return 0; 
}
ConfigurationClass Configuration;
