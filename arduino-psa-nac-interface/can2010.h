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
#ifndef CAN2010_H
#define CAN2010_H

#include <mcp2515.h>
#include "canmessage.h"

class Can2010Class {
  public:
    void receive();
    void setupCan();
    void send(can_message *message);
    void send0x228();
    void send0x3f6();
    void send0x525();
    void send0x167();
    void send0x15B();
    Can2010Class();
    void setupVersion();
  protected:
  private:
    MCP2515* CAN2;
    void process(can_message *message);
};

extern Can2010Class Can2010;

#endif
