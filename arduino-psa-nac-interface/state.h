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
#ifndef STATE_H
#define STATE_H

#include <Arduino.h>

class StateClass {
  public:
    StateClass();
    void setEconomyMode(bool state);
    bool getEconomyMode();
    void setEngineRunning(bool state);
    bool getEngineRunning();
    void setIgnition(bool state);
    bool getIgnition();
    void setTemperature(byte value);
    byte getTemperature();
    int getButtonState();
    void setButtonState(int value);
    int getLastButtonState();
    void setLastButtonState(int value);
    unsigned long getLastDebounceTime();
    void setLastDebounceTime(unsigned long value);
    unsigned long getButtonPushTime();
    void setButtonPushTime(unsigned long value);
    unsigned long getButtonSendTime();
    void setButtonSendTime(unsigned long value);
    unsigned long getDebounceDelay();
    void setDebounceDelay(unsigned long value);
    byte getScrollValue();
    void setScrollValue(byte value);
  protected:
  private:
    bool economyMode = false;
    bool engineRunning = false;
    bool ignition = false;
    byte temperature = 0;
    int buttonState = 0;
    int lastButtonState = 0;
    unsigned long lastDebounceTime = 0;
    unsigned long buttonPushTime = 0;
    unsigned long buttonSendTime = 0;
    unsigned long debounceDelay = 100;
    byte scrollValue = 0;
};

extern StateClass State;

#endif
