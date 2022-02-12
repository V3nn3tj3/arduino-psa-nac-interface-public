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
#include "state.h"
#include "storage.h"

StateClass::StateClass() {
  message15B->can_id = 0x15B;
  message15B->can_dlc = 8;
  memcpy(message15B->data, Storage.getMessage15B(), 8);
}

void StateClass::setEconomyMode(bool state) {
  economyMode = state;
}
bool StateClass::getEconomyMode() {
  return economyMode;
}

void StateClass::setEngineRunning(bool state) {
  engineRunning = state;
}
bool StateClass::getEngineRunning() {
  return engineRunning;
}

void StateClass::setIgnition(bool state) {
  ignition = state;
}
bool StateClass::getIgnition() {
  return ignition;
}

void StateClass::setTemperature(byte value) {
  temperature = value;
}
byte StateClass::getTemperature() {
  return temperature;
}

int StateClass::getButtonState() {
  return buttonState;
}

void StateClass::setButtonState(int value) {
  buttonState = value;
}
int StateClass::getLastButtonState() {
  return lastButtonState;
}
void StateClass::setLastButtonState(int value) {
  lastButtonState = value;
}
unsigned long StateClass::getLastDebounceTime() {
  return lastDebounceTime;
}
void StateClass::setLastDebounceTime(unsigned long value) {
  lastDebounceTime = value;
}
unsigned long StateClass::getButtonPushTime() {
  return buttonPushTime;
}
void StateClass::setButtonPushTime(unsigned long value) {
  buttonPushTime = value;
}
unsigned long StateClass::getButtonSendTime() {
  return buttonSendTime;
}
void StateClass::setButtonSendTime(unsigned long value) {
  buttonSendTime = value;
}
unsigned long StateClass::getDebounceDelay() {
  return debounceDelay;
}
void StateClass::setDebounceDelay(unsigned long value) {
  debounceDelay = value;
}

byte StateClass::getScrollValue() {
  return scrollValue;
}
void StateClass::setScrollValue(byte value) {
  scrollValue = value;
}

can_message* StateClass::getMessage15B() {
  return message15B;
}
can_message* StateClass::getMessage361() {
  return message361;
}

StateClass State;
