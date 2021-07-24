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
#include "input.h"
#include "configuration.h"
#include "state.h"
#include "can2010.h"
#include "canmessage.h"

InputClass::InputClass() {
  pinMode(Menu_PIN, INPUT_PULLUP);
  pinMode(VolumeDown_PIN, INPUT_PULLUP);
  pinMode(VolumeUp_PIN, INPUT_PULLUP);
}

void InputClass::process() {
  if (((millis() - State.getLastDebounceTime()) > State.getDebounceDelay())) {
    int tmpVal = 0;
    if (!digitalRead(Menu_PIN)) tmpVal += 0b001;
    if (!digitalRead(VolumeDown_PIN)) tmpVal += 0b010;
    if (!digitalRead(VolumeUp_PIN)) tmpVal += 0b100;
    if (tmpVal != State.getLastButtonState()) {
      State.setButtonPushTime(millis());
      State.setButtonSendTime(0);
    }
    if ((millis() - State.getButtonPushTime()) > 100) {
      switch (tmpVal) {
        case 0b001: {
            struct can_message new_message;
            new_message.data[0] = 0x02;
            new_message.data[1] = 0x00;
            new_message.data[2] = 0x00;
            new_message.data[3] = 0x00;
            new_message.data[4] = 0x00;
            new_message.data[5] = 0xFF;
            new_message.data[6] = 0x00;
            new_message.data[7] = 0x00;
            new_message.can_id = 0x122;
            new_message.can_dlc = 8;
            if (State.getButtonSendTime() == 0 || (millis() - State.getButtonPushTime() > 800 && (millis() - State.getButtonSendTime() > 800))) {
              Can2010.send(&new_message);
              State.setLastDebounceTime(millis());
              State.setButtonSendTime(millis());
#if SERIAL_ENABLED
              Serial.println(F("Send Menu"));
#endif
            }
          } break;
        case 0b010: {
            struct can_message new_message;
            new_message.data[0] = 0x04; //Volume down
            new_message.data[1] = State.getScrollValue();
            new_message.data[2] = 0x00;
            new_message.can_id = 0x21F;
            new_message.can_dlc = 3;
            if (State.getButtonSendTime() == 0 || (millis() - State.getButtonPushTime() > 800 && ((millis() - State.getButtonPushTime() < 2000 && millis() - State.getButtonSendTime() > 600) || (millis() - State.getButtonPushTime() > 2000 && millis() - State.getButtonSendTime() > 350)))) {
              Can2010.send(&new_message);
              State.setLastDebounceTime(millis());
              State.setButtonSendTime(millis());
#if SERIAL_ENABLED
              Serial.println(F("Send Vol-"));
#endif
            }
          } break;
        case 0b100: {
            struct can_message new_message;
            new_message.data[0] = 0x08; //Volume down
            new_message.data[1] = State.getScrollValue();
            new_message.data[2] = 0x00;
            new_message.can_id = 0x21F;
            new_message.can_dlc = 3;
            if (State.getButtonSendTime() == 0 || (millis() - State.getButtonPushTime() > 800 && ((millis() - State.getButtonPushTime() < 2000 && millis() - State.getButtonSendTime() > 600) || (millis() - State.getButtonPushTime() > 2000 && millis() - State.getButtonSendTime() > 350)))) {
              Can2010.send(&new_message);
              State.setLastDebounceTime(millis());
              State.setButtonSendTime(millis());
#if SERIAL_ENABLED
              Serial.println(F("Send Vol+"));
#endif
            }
          } break;
        case 0b110: {
            struct can_message new_message;
            new_message.data[0] = 0x0C; //Mute
            new_message.data[1] = State.getScrollValue();
            new_message.data[2] = 0x00;
            new_message.can_id = 0x21F;
            new_message.can_dlc = 3;
            if (State.getButtonSendTime() == 0) {
              Can2010.send(&new_message);
              State.setLastDebounceTime(millis());
              State.setButtonSendTime(millis());
#if SERIAL_ENABLED
              Serial.println(F("Send Mute"));
#endif
            }
          } break;
        default:
          State.setLastDebounceTime(millis());
      }
    }
    State.setLastButtonState(tmpVal);
  }
}

InputClass Input;
