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
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "storage.h"

#define SERIAL_ENABLED false
#define CS_PIN_CAN1 10
#define CS_PIN_CAN2 9
#define SERIAL_SPEED 115200
#define CAN_SPEED 125E3 // Entertainment CAN bus - Low speed
#define CAN_FREQ 8e6

#define EconomyModeEnabled true
#define ForceBrightness false
#define DistanceUnit "Km" //Km, Mi
#define VolumeUnit "L" //L, Gal
#define PressureUnit "Bar" //Bar, Psi

#define HasAnalogButtons true
#define Menu_PIN 4
#define VolumeDown_PIN 5
#define VolumeUp_PIN 6

#define EmulateVIN true
#define VINNumber "VF3XXXXXXXXXXXXXX"

class ConfigurationClass {
  public:
    ConfigurationClass();

    char getVINNumberChar(byte index);
    byte getLanguageId();
    void setLanguageId(byte value);
    byte getLanguageAndUnit();
    void setLanguageAndUnit(byte value);
    TemperatureReadingValues getTemperatureReading();
    void setTemperatureReading(TemperatureReadingValues value);
    FuelStatValues getFuelStat();
    void setFuelStat(FuelStatValues value);
    DateTime getDateTime();
    void setDateTime(DateTime value);
  protected:
  private:
};

extern ConfigurationClass Configuration;

#endif
