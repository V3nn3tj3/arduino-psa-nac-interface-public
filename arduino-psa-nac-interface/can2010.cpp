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
#include "can2004.h"
#include "can2010.h"
#include "canmessage.h"
#include "configuration.h"
#include "state.h"
#include <mcp2515.h>
#include <DS3232RTC.h>

Can2010Class::Can2010Class() {
}

void Can2010Class::setupCan() {
#if SERIAL_ENABLED
  Serial.println(F("Initialize CAN 2010"));
#endif
  CAN2 = new MCP2515(CS_PIN_CAN2);
  CAN2->reset();
  CAN2->setBitrate(CAN_SPEED, CAN_FREQ);
  while (CAN2->setNormalMode() != MCP2515::ERROR_OK) {
    delay(100);
  }
}

void Can2010Class::send(can_message *message) {
  CAN2->sendMessage(message);
#if SERIAL_ENABLED
  if (message->can_id == 0x260) {
    Serial.print(F("CAN2010 SEND    FRAME:ID="));
    char tmp[4];
    snprintf(tmp, 4, "%03X", message->can_id);
    Serial.print(tmp);
    Serial.print(F(":LEN="));
    Serial.print(message->can_dlc);
    for (int i = 0; i < message->can_dlc; i++) {
      Serial.print(F(":"));
      snprintf(tmp, 3, "%02X", message->data[i]);
      Serial.print(tmp);
    }
    Serial.println();
  }
#endif
}

void Can2010Class::receive() {
  struct can_message message;
  if (CAN2->readMessage(&message) == MCP2515::ERROR_OK) {
    process(&message);
#if SERIAL_ENABLED_CAN
    Serial.print(F("CAN2010 RECEIVE FRAME:ID="));
    Serial.print(message.can_id);
    Serial.print(F(":LEN="));
    Serial.print(message.can_dlc);
    char tmp[3];
    for (int i = 0; i < message.can_dlc; i++) {
      Serial.print(F(":"));
      snprintf(tmp, 3, "%02X", message.data[i]);
      Serial.print(tmp);
    }
    Serial.println();
#endif
  }
}

void Can2010Class::process(can_message *message) {
  switch (message->can_id) {
    case 0x39B: {
        setTime(message->data[3], message->data[4], 0, message->data[2], message->data[1], message->data[0] + 1872);
        RTC.set(now()); // Set the time on the RTC module too
        Configuration.setDateTime({ message->data[3], message->data[4], 0, message->data[2], message->data[1], message->data[0] + 1872 });

        struct can_message new_message;
        new_message.data[0] = hour();
        new_message.data[1] = minute();
        new_message.can_id = 0x228;
        new_message.can_dlc = 2;
        Can2004.send(&new_message);
      } break;
    case 0x15B: {
        //Update required to include LanguageId, also use bit from byte
        if (message->getFromByteBitOnPosition(0, 7)) {
          Configuration.setLanguageId(message->getFromByteBitsStartingFromFor(0, 3, 4));
          if (message->getFromByteBitOnPosition(1, 1)) {
            Configuration.setDistanceUnit(DistanceUnitValues::mi);
          } else {
            Configuration.setDistanceUnit(DistanceUnitValues::km);
          }
          if (message->getFromByteBitOnPosition(1, 0)) {
            Configuration.setConsumptionUnit(ConsumptionUnitValues::DistanceForUnit);
          } else {
            Configuration.setConsumptionUnit(ConsumptionUnitValues::VolFor100Units);
          }
          if (message->getFromByteBitOnPosition(1, 7)) {
            Configuration.setVolumeUnit(VolumeUnitValues::l);
          } else {
            Configuration.setVolumeUnit(VolumeUnitValues::gal);
          }
          if (message->getFromByteBitOnPosition(1, 6)) {
            Configuration.setTemperatureUnit(TemperatureUnitValues::F);
          } else {
            Configuration.setTemperatureUnit(TemperatureUnitValues::C);
          }
        }
      } break;
    case 0x1E5: {
        struct can_message new_message;
        // Ambience mapping
        switch (message->data[5]) {
          case 0x00: // User
            new_message.data[6] = 0x40;
            break;
          case 0x08: // Classical
            new_message.data[6] = 0x44;
            break;
          case 0x10: // Jazz
            new_message.data[6] = 0x48;
            break;
          case 0x18: // Pop-Rock
            new_message.data[6] = 0x4C;
            break;
          case 0x28: // Techno
            new_message.data[6] = 0x54;
            break;
          case 0x20: // Vocal
            new_message.data[6] = 0x50;
            break;
          default: // Default : User
            new_message.data[6] = 0x40;
        }
        // Loudness / Volume linked to speed
        switch (message->data[4]) {
          case 0x10: // Loudness / not linked to speed
            new_message.data[5] = 0x40;
            break;
          case 0x14: // Loudness / Volume linked to speed
            new_message.data[5] = 0x47;
            break;
          case 0x04:// No Loudness / Volume linked to speed
            new_message.data[5] = 0x07;
            break;
          case 0x00: // No Loudness / not linked to speed
            new_message.data[5] = 0x00;
            break;
          default: // Default : No Loudness / not linked to speed
            new_message.data[5] = 0x40;
        }

        // Bass
        // CAN2004 Telematic Range: (-9) "54" > (-7) "57" > ... > "72" (+9) ("63" = 0)
        // CAN2010 Telematic Range: "32" > "88" ("60" = 0)
        new_message.data[2] = ((message->data[2] - 32) / 4) + 57; // Converted value

        // Treble
        // CAN2004 Telematic Range: (-9) "54" > (-7) "57" > ... > "72" (+9) ("63" = 0)
        // CAN2010 Telematic Range: "32" > "88" ("60" = 0)
        new_message.data[4] = ((message->data[3] - 32) / 4) + 57; // Converted value

        // Balance - Left / Right
        // CAN2004 Telematic Range: (-9) "54" > (-7) "57" > ... > "72" (+9) ("63" = 0)
        // CAN2010 Telematic Range: "32" > "88" ("60" = 0)
        new_message.data[1] = ((message->data[1] - 32) / 4) + 57; // Converted value

        // Balance - Front / Back
        // CAN2004 Telematic Range: (-9) "54" > (-7) "57" > ... > "72" (+9) ("63" = 0)
        // CAN2010 Telematic Range: "32" > "88" ("60" = 0)
        new_message.data[0] = ((message->data[0] - 32) / 4) + 57; // Converted value

        // Mediums ?
        new_message.data[3] = 63; // 0x3F = 63

        new_message.can_id = message->can_id;
        new_message.can_dlc = message->can_dlc;
        Can2004.send(&new_message);
      } break;
    default:
      Can2004.send(message);
  }
}

//Time
//0x228  2 01  02
//Send 0x228 every 60000ms
void Can2010Class::send0x228() {
  struct can_message new_message;
  new_message.data[0] = hour();
  new_message.data[1] = minute();
  new_message.can_id = 0x228;
  new_message.can_dlc = 2;
  Can2004.send(&new_message);
}

void Can2010Class::send0x167() {
  struct can_message new_message;
  new_message.data[0] = 0x08;//0x00;
  new_message.data[1] = 0x10;//0x06;
  new_message.data[2] = 0xFF;
  new_message.data[3] = 0xFF;
  new_message.data[4] = 0x7F;
  new_message.data[5] = 0xFF;
  new_message.data[6] = 0x00;
  new_message.data[7] = 0x00;
  new_message.can_id = 0x167;
  new_message.can_dlc = 8;
  Can2004.send(&new_message);
}

//Send every 1000ms - Seen on car network
//Unfinished frame, need research
void Can2010Class::send0x3f6() {
  struct can_message new_message;
  unsigned long secondsInDay = Configuration.getSecondsFromDay();

  new_message.data[0] = (((1 << 8) - 1) & (secondsInDay >> (12)));
  new_message.data[1] = (((1 << 8) - 1) & (secondsInDay >> (4)));
  new_message.data[2] = (((((1 << 4) - 1) & (secondsInDay)) << 4)) + (((1 << 4) - 1) & (Configuration.getDayOfYear() >> (8)));
  new_message.data[3] = (((1 << 8) - 1) & (Configuration.getDayOfYear()));
  new_message.data[4] = Configuration.getYearsRunning();

  new_message.data[5] = 0x00;

  if (Configuration.getDistanceUnit() == DistanceUnitValues::mi) {
    new_message.setInByteBitOnPosition(5, 0, 1);
  }
  if (Configuration.getVolumeUnit() == VolumeUnitValues::gal) {
    new_message.setInByteBitOnPosition(5, 1, 1);
  }
  if (Configuration.getConsumptionUnit() == ConsumptionUnitValues::DistanceForUnit) {
    new_message.setInByteBitOnPosition(5, 2, 1);
  }
  if (strcmp(PressureUnit, "Psi") == 0) {
    new_message.setInByteBitOnPosition(5, 3, 1);
  }
  new_message.setInByteBitOnPosition(5, 4, 1);
  if (Configuration.getTemperatureUnit() == TemperatureUnitValues::F) {
    new_message.setInByteBitOnPosition(5, 5, 1);
  }
  new_message.setInByteBitOnPosition(5, 7, 1);
  new_message.data[6] = Configuration.getLanguageId();
  new_message.can_id = 0x3f6;
  new_message.can_dlc = 7;
  Can2004.send(&new_message);
}

void Can2010Class::send0x525() {
  struct can_message new_message;
  new_message.data[0] = 0x00;
  new_message.data[1] = 0x00;
  new_message.data[2] = 0x00;
  new_message.data[3] = 0x00;
  new_message.data[4] = 0x00;
  new_message.data[5] = 0x00;
  new_message.data[6] = 0x00;
  new_message.data[7] = 0x00;
  new_message.can_id = 0x525;
  new_message.can_dlc = 8;
  Can2004.send(&new_message);
}

void Can2010Class::send0x15B() {
  //if (State.getMessage15B()->data[0] != 0x00) {
  //  Can2004.send(State.getMessage15B());
  //}
}

void Can2010Class::setupVersion() {
  struct can_message new_message;
  new_message.data[0] = 0x25;
  new_message.data[1] = 0x1D;
  new_message.data[2] = 0x03;
  new_message.data[3] = 0x06;
  new_message.data[4] = 0x08;
  new_message.data[5] = 0x00;
  new_message.data[6] = 0x20;
  new_message.data[7] = 0x10;
  new_message.can_id = 0x5E5;
  new_message.can_dlc = 8;
  Can2004.send(&new_message);
  new_message.data[0] = 0x58;
  new_message.data[1] = 0xF8;
  new_message.data[2] = 0xEB;
  new_message.data[3] = 0x80;
  new_message.data[4] = 0x01;
  new_message.data[5] = 0x02;
  new_message.data[6] = 0x05;
  new_message.data[7] = 0x07;
  new_message.can_id = 0x4A5;
  new_message.can_dlc = 8;
  /*
    Can2004.send(&new_message);
    new_message.data[0] = 0x05;
    new_message.data[1] = 0x00;
    new_message.data[2] = 0x00;
    new_message.data[3] = 0x00;
    new_message.data[4] = 0x00;
    new_message.data[5] = 0x00;
    new_message.data[6] = 0x00;
    new_message.data[7] = 0xF0;
    new_message.can_id = 0x15B;
    new_message.can_dlc = 8;
    Can2004.send(&new_message);
  */
}

//Display
//0x15B  8 01  53  B2  C0  00  D0  00  F0
//0x167  8 08  00  00  00  7F  FF  00  00
//Time
//0x3F6  7 00  E8  30  00  00  90  06

Can2010Class Can2010;
