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
#include <CAN.h>
#include <DS3232RTC.h>

Can2010Class::Can2010Class() {
  CAN2.setPins(CS_PIN_CAN2, 2);
  CAN2.setClockFrequency(CAN_FREQ);
  CAN2.begin(CAN_SPEED);
}

void Can2010Class::send(can_message *message) {
  if (message->len > 0) {
    CAN2.beginPacket(message->id, message->len);// or CAN.beginExtendedPacket(0x18db33f1, 8);
    for (int i = 0; i < message->len; i++) {
      CAN2.write(message->data[i]);
    }
    CAN2.endPacket();
  }
#if SERIAL_ENABLED
  Serial.print("CAN2010 SEND    FRAME:ID=");
  char tmp[4];
  snprintf(tmp, 4, "%03X", message->id);
  Serial.print(tmp);
  Serial.print(":LEN=");
  Serial.print(message->len);
  for (int i = 0; i < message->len; i++) {
    Serial.print(":");
    snprintf(tmp, 3, "%02X", message->data[i]);
    Serial.print(tmp);
  }
  Serial.println();
#endif
}

void Can2010Class::receive() {
  int packetSize = CAN2.parsePacket();

  if (packetSize && packetSize <= 8) {
    struct can_message message;

    message.id = CAN2.packetId();
    if (!CAN2.packetRtr()) {
      //Serial.print(" and requested length ");
      //Serial.println(CAN.packetDlc());
      //} else {
      //Serial.print(" and length ");
      message.len = packetSize;
      byte packetIndex = 0;
      while (CAN2.available()) {
        message.data[packetIndex++] = CAN2.read();
      }
#if SERIAL_ENABLED
      Serial.print("CAN2010 RECEIVE FRAME:ID=");
      Serial.print(message.id);
      Serial.print(":LEN=");
      Serial.print(message.len);
      char tmp[3];
      for (int i = 0; i < message.len; i++) {
        Serial.print(":");
        snprintf(tmp, 3, "%02X", message.data[i]);
        Serial.print(tmp);
      }
      Serial.println();
#endif
      process(&message);
    }
#if SERIAL_ENABLED
  } else if (packetSize) {
    Serial.print("CAN2010 RECEIVE FRAME::ID=");
    Serial.print(CAN2.packetId());
    Serial.println(":WRONG PACKET");
#endif
  }
}

void Can2010Class::process(can_message *message) {
  switch (message->id) {
    case 0x39B: {
        setTime(message->data[4], message->data[3], 0, message->data[2], message->data[1], message->data[0] + 1872);
        RTC.set(now()); // Set the time on the RTC module too
        Configuration.setDateTime({ message->data[4], message->data[3], 0, message->data[2], message->data[1], message->data[0] + 1872 });

        struct can_message new_message;
        new_message.data[0] = hour();
        new_message.data[1] = minute();
        new_message.id = 0x228;
        new_message.len = 2;
        Can2004.send(&new_message);
      } break;
    case 0x15B: {
        //Update required to include LanguageId, also use bit from byte
        if (message->getFromByteBitOnPosition(0, 7)) {
          Configuration.setLanguageAndUnit(message->data[0]);
          if (message->getFromByteBitOnPosition(1, 7)) {
            Configuration.setFuelStat(FuelStatValues::mpgMi);
          } else {
            Configuration.setFuelStat(FuelStatValues::Lkm);
          }
          if (message->getFromByteBitOnPosition(1, 6)) {
            Configuration.setTemperatureReading(TemperatureReadingValues::F);
          } else {
            Configuration.setTemperatureReading(TemperatureReadingValues::C);
          }
        } else {
          if (message->getFromByteBitOnPosition(1, 7)) {
            Configuration.setLanguageId(ceil(message->data[1] / 4.0) - 1);
          } else {
            Configuration.setLanguageId(ceil(message->data[1] / 4.0));
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

        new_message.id = message->id;
        new_message.len = message->len;
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
  new_message.id = 0x228;
  new_message.len = 2;
  Can2004.send(&new_message);
}
//Send every 1000ms - Seen on car network
//Unfinished frame, need research
void Can2010Class::send0x3f6() {
  struct can_message new_message;
  unsigned long secondsInDay = hour() * 3600 + minute() * 60 + second();
  secondsInDay = secondsInDay << 4;
  new_message.data[0] = (secondsInDay >> (8 * 2)) & 0xff;
  new_message.data[1] = (secondsInDay >> (8 * 1)) & 0xff;
  new_message.data[2] = (secondsInDay >> (8 * 0)) & 0xff;
  new_message.data[3] = 0x00;
  new_message.data[4] = 0x00;

  new_message.data[5] = 0x00;
  if (DistanceUnit == "Mi") {
    new_message.setInByteBitOnPosition(5, 0, 1);
  }
  if (VolumeUnit == "Gal") {
    new_message.setInByteBitOnPosition(5, 1, 1);
  }
  if (Configuration.getFuelStat() == FuelStatValues::mpgMi) {
    new_message.setInByteBitOnPosition(5, 2, 1);
  }
  if (PressureUnit == "Psi") {
    new_message.setInByteBitOnPosition(5, 3, 1);
  }
  new_message.setInByteBitOnPosition(5, 4, 1);
  if (Configuration.getTemperatureReading() == TemperatureReadingValues::F) {
    new_message.setInByteBitOnPosition(5, 5, 1);
  }
  new_message.setInByteBitOnPosition(5, 7, 1);
  new_message.data[6] = Configuration.getLanguageId();
  new_message.id = 0x3f6;
  new_message.len = 7;
  Can2004.send(&new_message);
}

//Display
//0x15B  8 01  53  B2  C0  00  D0  00  F0
//0x167  8 08  00  00  00  7F  FF  00  00
//Time
//0x3F6  7 00  E8  30  00  00  90  06

Can2010Class Can2010;
