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
#include <mcp2515.h>

#define SERIAL_ENABLED false
#define SERIAL_ENABLED_CAN false
#define CS_PIN_CAN1 10
#define CS_PIN_CAN2 9
#define SERIAL_SPEED 250000
#define CAN_SPEED CAN_125KBPS // Entertainment CAN bus - Low speed
#define CAN_FREQ MCP_8MHZ

#define EconomyModeEnabled false
#define PassTrue false
#define ForceBrightness false
#define PressureUnit "Bar" //Bar, Psi

#define HasAnalogButtons true
#define Menu_PIN 4
#define VolumeDown_PIN 5
#define VolumeUp_PIN 6

#define EmulateVIN true
#define VINNumber "VF7SAHNPSKWXXXXXX"

class ConfigurationClass {
  public:
    ConfigurationClass();

    char getVINNumberChar(byte index);
    byte getLanguageId();
    void setLanguageId(byte value);
    bool getFromLanguageBitOnPosition(byte position);
    TemperatureUnitValues getTemperatureUnit();
    void setTemperatureUnit(TemperatureUnitValues value);
    DistanceUnitValues getDistanceUnit();
    void setDistanceUnit(DistanceUnitValues value);
    ConsumptionUnitValues getConsumptionUnit();
    void setConsumptionUnit(ConsumptionUnitValues value);
    VolumeUnitValues getVolumeUnit();
    void setVolumeUnit(VolumeUnitValues value);
    DateTime getDateTime();
    void setDateTime(DateTime value);
    int getDayOfYear();
    unsigned long getSecondsFromDay();
    int getYearsRunning();
  protected:
  private:
};

extern ConfigurationClass Configuration;

#endif
