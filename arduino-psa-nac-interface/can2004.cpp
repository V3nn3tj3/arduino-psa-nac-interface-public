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
#include <DS3232RTC.h>
#include <mcp2515.h>

Can2004Class::Can2004Class() {
}

void Can2004Class::setupCan() {
#if SERIAL_ENABLED_CAN
  Serial.println(F("Initialize CAN 2004"));
#endif
  CAN1 = new MCP2515(CS_PIN_CAN1);
  CAN1->reset();
  CAN1->setBitrate(CAN_SPEED, CAN_FREQ);
  while (CAN1->setNormalMode() != MCP2515::ERROR_OK) {
    delay(100);
  }
}

void Can2004Class::send(can_message *message) {
  CAN1->sendMessage(message);
#if SERIAL_ENABLED
  if (message->can_id == 260) {
    Serial.print(F("CAN2004 SEND    FRAME:ID="));
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

void Can2004Class::receive() {
  struct can_message message;
  if (CAN1->readMessage(&message) == MCP2515::ERROR_OK) {
    process(&message);
#if SERIAL_ENABLED
    if (message.can_id == 0x260) {
      Serial.print(F("CAN2004 RECEIVE FRAME:ID="));
      char tmp[4];
      snprintf(tmp, 4, "%03X", message.can_id);
      Serial.print(tmp);
      Serial.print(F(":LEN="));
      Serial.print(message.can_dlc);
      for (int i = 0; i < message.can_dlc; i++) {
        Serial.print(F(":"));
        snprintf(tmp, 3, "%02X", message.data[i]);
        Serial.print(tmp);
      }
      Serial.println();
    }
#endif
  }
}

void Can2004Class::process(can_message *message) {
  switch (message->can_id) {
    case 0x36: {
        //if (message->data[2] >= 0x80) {
        if (message->getFromByteBitOnPosition(2, 7)) {
          State.setEconomyMode(true);
        } else {
          State.setEconomyMode(false);
        }
        Can2010.send(message);
      } break;
    //Running engine detection
    case 0xb6: {
        if (message->data[0] > 0x00 || message->data[1] > 0x00) { // Engine RPM, 0x00 0x00 when the engine is OFF
          State.setEngineRunning(true);
        } else {
          State.setEngineRunning(false);
        }
        Can2010.send(message);
      } break;
      //Emulate VIN
#if defined(EmulateVIN)
    case 0x336: {
        struct can_message new_message;
        new_message.can_id = message->can_id;
        new_message.can_dlc = 3;
        new_message.data[0] = Configuration.getVINNumberChar(0); //V
        new_message.data[1] = Configuration.getVINNumberChar(1); //F
        new_message.data[2] = Configuration.getVINNumberChar(2); //3
        Can2010.send(&new_message);
      } break;
    case 0x3b6: {
        struct can_message new_message;
        new_message.can_id = message->can_id;
        new_message.can_dlc = 6;
        new_message.data[0] = Configuration.getVINNumberChar(3); //
        new_message.data[1] = Configuration.getVINNumberChar(4); //
        new_message.data[2] = Configuration.getVINNumberChar(5); //
        new_message.data[3] = Configuration.getVINNumberChar(6); //
        new_message.data[4] = Configuration.getVINNumberChar(7); //
        new_message.data[5] = Configuration.getVINNumberChar(8); //
        Can2010.send(&new_message);
      } break;
    case 0x2b6: {
        struct can_message new_message;
        new_message.can_id = message->can_id;
        new_message.can_dlc = 8;
        new_message.data[0] = Configuration.getVINNumberChar(9); //
        new_message.data[1] = Configuration.getVINNumberChar(10); //
        new_message.data[2] = Configuration.getVINNumberChar(11); //
        new_message.data[3] = Configuration.getVINNumberChar(12); //
        new_message.data[4] = Configuration.getVINNumberChar(13); //
        new_message.data[5] = Configuration.getVINNumberChar(14); //
        new_message.data[6] = Configuration.getVINNumberChar(15); //
        new_message.data[7] = Configuration.getVINNumberChar(16); //
        Can2010.send(&new_message);
      } break;
#endif
    //Stalk control
    case 0x21f: {
        State.setScrollValue(message->data[1]);
        Can2010.send(message);
      } break;
    //Aircon
    case 0x1d0: {
        if (State.getEngineRunning()) {
          struct can_message new_message;
          byte leftTemp = 0;
          byte rightTemp = 0;
          bool mono = false;
          bool fanOff = false;
          byte fanSpeed = 0;
          bool deMist = false;
          bool airRecycle = false;
          bool autoFan = false;
          bool airConditioningON = false;

          byte fanPosition = 0;

          getAirconTemp(message, leftTemp, rightTemp, mono);
          getAirconFan(message, fanOff, fanSpeed);
          getAirconFanPosition(message, fanPosition);
          getAirconSettings(message, deMist, airRecycle, autoFan, fanOff, airConditioningON);

          if (deMist) {
            fanSpeed = 0x10;
            fanPosition = fanPosition + 16;
          } else if (autoFan) {
            fanSpeed = 0x10;
          }

          if (fanOff) {
            airConditioningON = false;
            fanSpeed = 0x41;
            leftTemp = 0x00;
            rightTemp = 0x00;
            fanPosition = 0x04;
          }

          if (airConditioningON) {
            new_message.data[0] = 0x01; // A/C ON - Auto Soft : "00" / Auto Normal "01" / Auto Fast "02"
          } else {
            new_message.data[0] = 0x09; // A/C OFF - Auto Soft : "08" / Auto Normal "09" / Auto Fast "0A"
          }

          new_message.data[1] = 0x00;
          new_message.data[2] = 0x00;
          new_message.data[3] = leftTemp;
          new_message.data[4] = rightTemp;
          new_message.data[5] = fanSpeed;
          new_message.data[6] = fanPosition;
          new_message.data[7] = 0x00;
          new_message.can_id = 0x350;
          new_message.can_dlc = 8;
          Can2010.send(&new_message);
        }
      } break;
    //Ignition detection + temperature save
    case 0x0f6: {
        //if (message->data[0] > 0x80) {
        if (message->getFromByteBitOnPosition(0, 3)) {
          State.setIgnition(true);
        } else {
          State.setIgnition(false);
        }

        byte temperature = ceil(message->data[5]  / 2.0) - 40; // Temperatures can be negative but we only have 0 > 255, the new range is starting from -40°C
        if (State.getTemperature() != temperature) {
          State.setTemperature(temperature);
        }

        Can2010.send(message);
      } break;
    //Instrument cluster
    case 0x168: {
        struct can_message new_message;
        new_message.data[0] = message->data[0];
        new_message.data[1] = message->data[1];
        new_message.data[2] = message->data[5]; // Investigation to do
        new_message.data[3] = message->data[3];
        new_message.data[4] = message->data[5]; // Investigation to do
        new_message.data[5] = message->data[5]; // Investigation to do
        new_message.data[6] = message->data[6];
        new_message.data[7] = message->data[7];
        new_message.can_id = 0x168;
        new_message.can_dlc = 8;
        Can2010.send(&new_message);
      } break;
    //Instrument cluster
    case 0x128: {
        struct can_message new_message;
        new_message.data[0] = message->data[4];
        new_message.data[1] = message->data[6];
        new_message.data[2] = message->data[7];
        new_message.data[3] = 0x00;
        if (message->getFromByteBitOnPosition(0, 5)) { //Handbrake
          new_message.setInByteBitOnPosition(3, 1, 1);
        }
        new_message.data[4] = 0x00; // Investigation to do
        new_message.data[5] = 0x00; // Investigation to do
        new_message.data[6] = 0x04;
        new_message.data[7] = 0x00;
        new_message.can_id = 0x168;
        new_message.can_dlc = 8;
        Can2010.send(&new_message);
      } break;
    //Maintenance
    case 0x3a7: {
        struct can_message new_message;
        new_message.data[0] = 0x40;
        new_message.data[1] = message->data[5];
        new_message.data[2] = message->data[6];
        new_message.data[3] = message->data[3];
        new_message.data[4] = message->data[4];
        new_message.can_id = 0x3e7;
        new_message.can_dlc = 5;
        Can2010.send(&new_message);
      } break;
    //Cruise control
    case 0x1a8: {
        Can2010.send(message);

        struct can_message new_message;
        new_message.data[0] = message->data[1];
        new_message.data[1] = message->data[2];
        new_message.data[2] = message->data[0];
        new_message.data[3] = 0x80;
        new_message.data[4] = 0x14;
        new_message.data[5] = 0x7f;
        new_message.data[6] = 0xff;
        new_message.data[7] = 0x98;
        new_message.can_id = 0x228;
        new_message.can_dlc = 8;
        Can2010.send(&new_message);
      } break;
    case 0x361: {
        // Work in progress, do not forward possible old personalization settings from CAN2004
      } break;
    case 0x260: {
        struct can_message new_message;
        new_message.setInByteBitOnPosition(0, 7, 1);
        new_message.setInByteBitOnPosition(0, 5, Configuration.getFromLanguageBitOnPosition(3));
        new_message.setInByteBitOnPosition(0, 4, Configuration.getFromLanguageBitOnPosition(2));
        new_message.setInByteBitOnPosition(0, 3, Configuration.getFromLanguageBitOnPosition(1));
        new_message.setInByteBitOnPosition(0, 2, Configuration.getFromLanguageBitOnPosition(0));
        if (Configuration.getDistanceUnit() == DistanceUnitValues::mi) {
          new_message.setInByteBitOnPosition(0, 1, 1);
        }
        if (Configuration.getVolumeUnit() == VolumeUnitValues::gal) {
          new_message.setInByteBitOnPosition(1, 7, 1);
        }
        if (Configuration.getConsumptionUnit() == ConsumptionUnitValues::DistanceForUnit) {
          new_message.setInByteBitOnPosition(0, 0, 1);
        }
        if (Configuration.getTemperatureUnit() == TemperatureUnitValues::F) {
          new_message.setInByteBitOnPosition(1, 6, 1);
        }

        new_message.data[2] = 0x00;
        new_message.data[3] = 0x00;
        new_message.data[4] = 0x00;
        new_message.data[5] = 0x00;
        new_message.data[6] = 0x00;
        new_message.data[7] = 0x00;
        new_message.can_id = 0x260;
        new_message.can_dlc = 7;
        Can2010.send(&new_message);
      } break;
    default:
      Can2010.send(message);
  }
}

//Send every 200ms
void Can2004Class::send0x122() {
  struct can_message new_message;
  new_message.data[0] = 0x00;
  new_message.data[1] = 0x00;
  new_message.data[2] = 0x00;
  new_message.data[3] = 0x00;
  new_message.data[4] = 0x00;
  new_message.data[5] = 0x02;
  new_message.data[6] = 0x00;
  new_message.data[7] = 0x00;
  new_message.can_id = 0x122;
  new_message.can_dlc = 8;
  Can2010.send(&new_message);
}

//Send every 1000ms - Seen on car network
void Can2004Class::send0x169() {
  struct can_message new_message;
  new_message.data[0] = 0x00;
  new_message.data[1] = 0xFF;
  new_message.data[2] = 0xFF;
  new_message.data[3] = 0xFF;
  new_message.data[4] = 0xFF;
  new_message.data[5] = 0xFF;
  new_message.data[6] = 0xFF;
  new_message.data[7] = 0xFF;
  new_message.can_id = 0x169;
  new_message.can_dlc = 8;
  Can2010.send(&new_message);
}

//Send every 1000ms - Seen on car network
void Can2004Class::send0x361() {
  //if (State.getMessage361()->data[0] != 0x00) {
  //  Can2010.send(State.getMessage361());
  //}
}

//Send every 500ms
void Can2004Class::send0x236() {
  struct can_message new_message;

  new_message.data[0] = 0x04; //Standard - No Economy Mode, No running engine
  new_message.data[5] = 0x0F; //Standard - No Economy Mode, Ignition on
  if (State.getEconomyMode() && EconomyModeEnabled) {
    new_message.setInByteBitOnPosition(0, 4, 1); //Enable economy mode
    new_message.setInByteBitOnPosition(5, 0, 0); //Enable economy mode
    if (!State.getIgnition()) {
      new_message.setInByteBitOnPosition(5, 1, 0); //Remove ignition
    }
  }
  if (State.getEngineRunning()) {
    new_message.setInByteBitOnPosition(0, 6, 1); //Set engine running
  }

  new_message.data[1] = 0x03;
  new_message.data[2] = 0xDE;

  new_message.data[3] = 0x00; // Increasing value,
  new_message.data[4] = 0x00; // counter ?

  new_message.data[6] = 0xFE;
  new_message.data[7] = 0x00;
  new_message.can_id = 0x236;
  new_message.can_dlc = 8;
  Can2010.send(&new_message);
}


//Send every 1000ms
void Can2004Class::send0x276() {
  struct can_message new_message;

  if (timeStatus() == timeSet) {
    new_message.data[0] = (year() - 1872); // Year would not fit inside one byte (0 > 255), substract 1872 and you get this new range (1872 > 2127)
    new_message.data[1] = month();
    new_message.data[2] = day();
    new_message.data[3] = hour();
    new_message.data[4] = minute();
    new_message.data[5] = 0x3F;
    new_message.data[6] = 0xFE;
  } else {
    new_message.data[0] = (Configuration.getDateTime().Year - 1872); // Year would not fit inside one byte (0 > 255), substract 1872 and you get this new range (1872 > 2127)
    new_message.data[1] = Configuration.getDateTime().Month;
    new_message.data[2] = Configuration.getDateTime().Day;
    new_message.data[3] = Configuration.getDateTime().Hour;
    new_message.data[4] = Configuration.getDateTime().Minute;
    new_message.data[5] = 0x3F;
    new_message.data[6] = 0xFE;
  }
  new_message.can_id = 0x276;
  new_message.can_dlc = 7;
  Can2010.send(&new_message);
}

//Send every 500ms
void Can2004Class::send0x350() {
  if (!State.getEngineRunning()) {
    struct can_message new_message;

    new_message.data[0] = 0x09;
    new_message.data[1] = 0x00;
    new_message.data[2] = 0x00;
    new_message.data[3] = 0x00; //LeftTemp
    new_message.data[4] = 0x00; //RightTemp
    new_message.data[5] = 0x41; //FanSpeed
    new_message.data[6] = 0x04; //FanPosition
    new_message.data[7] = 0x00;
    new_message.can_id = 0x350;
    new_message.can_dlc = 8;
    Can2010.send(&new_message);
  }
}

//Helpers
void Can2004Class::getAirconTemp(can_message *message, byte &leftTemp, byte &rightTemp, bool &mono) {
  leftTemp = message->data[5];
  rightTemp = message->data[6];
  if (leftTemp == rightTemp) { // No other way to detect MONO mode
    mono = true;
    leftTemp += 64;
  }
}

void Can2004Class::getAirconFan(can_message *message, bool &fanOff, byte &fanSpeed) {
  // Fan Speed BSI_2010 = "41" (Off) > "49" (Full speed)
  if (message->data[2] == 15) {
    fanOff = true;
    fanSpeed = 0x41;
  } else {
    fanSpeed = (message->data[2] + 66);
  }
}
void Can2004Class::getAirconFanPosition(can_message *message, byte &fanPosition) {
  bool footwellFan = false;
  bool windscreenFan = false;
  bool centralFan = false;
  switch (message->data[3]) {
    case 0x40:
      footwellFan = false;
      windscreenFan = true;
      centralFan = false;
      break;
    case 0x30:
      footwellFan = false;
      windscreenFan = false;
      centralFan = true;
      break;
    case 0x20:
      footwellFan = true;
      windscreenFan = false;
      centralFan = false;
      break;
    case 0x70:
      footwellFan = false;
      windscreenFan = true;
      centralFan = true;
      break;
    case 0x80:
      footwellFan = true;
      windscreenFan = true;
      centralFan = true;
      break;
    case 0x50:
      footwellFan = true;
      windscreenFan = false;
      centralFan = true;
      break;
    case 0x10:
      footwellFan = false;
      windscreenFan = false;
      centralFan = false;
      break;
    case 0x60:
      footwellFan = true;
      windscreenFan = true;
      centralFan = false;
      break;
    default:
      footwellFan = false;
      windscreenFan = false;
      centralFan = false;
  }

  if (!footwellFan && !windscreenFan && centralFan) {
    fanPosition = 0x34;
  } else if (footwellFan && windscreenFan && centralFan) {
    fanPosition = 0x84;
  } else if (!footwellFan && windscreenFan && centralFan) {
    fanPosition = 0x74;
  } else if (footwellFan && !windscreenFan && centralFan) {
    fanPosition = 0x54;
  } else if (footwellFan && !windscreenFan && !centralFan) {
    fanPosition = 0x24;
  } else if (!footwellFan && windscreenFan && !centralFan) {
    fanPosition = 0x44;
  } else if (footwellFan && windscreenFan && !centralFan) {
    fanPosition = 0x64;
  } else {
    fanPosition = 0x04; // Nothing
  }
}

void Can2004Class::getAirconSettings(can_message *message, bool &deMist, bool &airRecycle, bool &autoFan, bool &fanOff, bool &airConditioningON) {
  switch (message->data[4]) {
    case 0x10:
      deMist = true;
      airRecycle = false;
      break;
    case 0x30:
      airRecycle = true;
      break;
    default:
      airRecycle = false;
  }

  switch (message->data[0]) {
    case 0x11:
      deMist = true;
      airConditioningON = true;
      fanOff = false;
    case 0x12:
      deMist = true;
      airConditioningON = false;
      fanOff = false;
      break;
    case 0x21:
      deMist = true;
      airConditioningON = true;
      fanOff = false;
      break;
    case 0xa2:
      fanOff = true;
      airConditioningON = false;
      break;
    case 0x22:
      airConditioningON = false;
      break;
    case 0x20:
      airConditioningON = true;
      break;
    case 0x02:
      airConditioningON = false;
      autoFan = false;
      break;
    case 0x00:
      airConditioningON = true;
      autoFan = true;
      break;
  }
}

Can2004Class Can2004;
