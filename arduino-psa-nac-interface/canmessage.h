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

#include <stdint.h>

typedef unsigned char __u8;
typedef unsigned short __u16;
typedef unsigned long __u32;

#define CAN_MAX_DLC 8
#define CAN_MAX_DLEN 8

struct can_message {
  long  id;
  byte  len;
  int  data[CAN_MAX_DLEN] __attribute__((aligned(8)));

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
};

#endif
