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
#ifndef CAN2004_H
#define CAN2004_H

#include <CAN.h>
#include "canmessage.h"

class Can2004Class {
  private:
    MCP2515Class CAN1;
    void process(can_message *message);

    //Helpers
    void getAirconTemp(can_message *message, byte &leftTemp, byte &rightTemp, bool &mono);
    void getAirconFan(can_message *message, bool &fanOff, byte &fanSpeed);
    void getAirconFanPosition(can_message *message, byte &fanPosition);
    void getAirconSettings(can_message *message, bool &deMist, bool &airRecycle, bool &autoFan, bool &fanOff, bool &airConditioningON);
  protected:
  public:
    void receive();
    void send(can_message *message);
    Can2004Class();
    void send0x122();
    void send0x169();
    void send0x236();
    void send0x276();
    void send0x350();
};

extern Can2004Class Can2004;

#endif