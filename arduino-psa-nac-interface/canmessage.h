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
#ifndef CANMESSAGE_H_
#define CANMESSAGE_H_

#include <mcp2515.h>

struct can_message : can_frame {
  can_message()
  {
    can_id = 0x00;
    can_dlc = 0;
    for (int i = 0; i < 8; i++) {
      data[i] = 0x00;
    }
  }

  bool getFromByteBitOnPosition(byte byteNumber, byte position) {
    return (data[byteNumber] >> position) & 0x1;
  }

  void setInByteBitOnPosition(byte byteNumber, byte position, bool value) {
    if (value) {
      data[byteNumber] |= (1 << position);
    } else {
      data[byteNumber] &= ~(1 << position);
    }
  }

  int getFromByteBitsStartingFromFor(byte byteNumber, byte startposition, int length)
  {
    return (((1 << length) - 1) & (data[byteNumber] >> (startposition - 1)));
  }
};

#endif
