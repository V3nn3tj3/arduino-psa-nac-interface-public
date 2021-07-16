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
#pragma once
#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>

#define LanguageID 6

struct DateTime {
  uint8_t Second;
  uint8_t Minute;
  uint8_t Hour;
  uint8_t Day;
  uint8_t Month;
  unsigned int Year;
};

enum TemperatureReadingValues { C, F };
enum FuelStatValues { Lkm, mpgMi, kmL };

struct Storage {
  byte version;
  DateTime dateTime;
  byte languageId;
  byte languageAndUnit;
  TemperatureReadingValues temperatureReading;
  FuelStatValues fuelStat;
};

class StorageClass {
  public:
    StorageClass();

    DateTime getDateTime();
    void setDateTime(DateTime value);
    byte getLanguageId();
    void setLanguageId(byte value);
    byte getLanguageAndUnit();
    void setLanguageAndUnit(byte value);
    TemperatureReadingValues getTemperatureReading();
    void setTemperatureReading(TemperatureReadingValues value);
    FuelStatValues getFuelStat();
    void setFuelStat(FuelStatValues value);
  protected:
  private:
    //DateTime
    struct Storage storage;
    void saveToStorage();
    void loadFromStorage();
};

extern StorageClass Storage;

#endif
