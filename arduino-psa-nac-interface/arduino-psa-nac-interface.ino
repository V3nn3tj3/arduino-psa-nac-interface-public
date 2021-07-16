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

#include <CAN.h>
#include <Thread.h>
#include <ThreadController.h>
#include "configuration.h"
#include "can2004.h"
#include "can2010.h"
#if HasAnalogButtons
#include "input.h"
#endif
#include <Time.h>
#include <TimeLib.h>
#include <DS3232RTC.h>

ThreadController controller = ThreadController();

void setup() {
#if SERIAL_ENABLED
  Serial.begin(SERIAL_SPEED);
#endif
  setSyncProvider(RTC.get);
  if (timeStatus() != timeSet) {
#if SERIAL_ENABLED
    Serial.println("Unable to sync with the RTC");
#endif
    //set/sync time
    setTime(Configuration.getDateTime().Hour, Configuration.getDateTime().Minute, 0, Configuration.getDateTime().Day, Configuration.getDateTime().Month, Configuration.getDateTime().Year);
  }
#if SERIAL_ENABLED
  else {
    Serial.println("RTC has set the system time");
  }
#endif

#if HasAnalogButtons
  Thread inputThreat = Thread();
#endif
  Thread Can2004Threat = Thread();
  Thread Can2010Threat = Thread();

  Thread Can2004send0x122 = Thread();
  Thread Can2004send0x169 = Thread();
  Thread Can2004send0x236 = Thread();
  Thread Can2004send0x276 = Thread();
  Thread Can2004send0x350 = Thread();

  Thread Can2010send0x228 = Thread();
  Thread Can2010send0x3f6 = Thread();

#if HasAnalogButtons
  inputThreat.enabled = true;
  inputThreat.onRun(Input.process);
  controller.add(&inputThreat);
#endif
  Can2004Threat.enabled = true;
  Can2004Threat.onRun([&Can2004] { Can2004.receive(); });
  controller.add(&Can2004Threat);
  Can2010Threat.enabled = true;
  Can2010Threat.onRun([&Can2010] { Can2010.receive(); });
  controller.add(&Can2010Threat);

  Can2004send0x122.enabled = true;
  Can2004send0x122.setInterval(200);
  Can2004send0x122.onRun([&Can2004] { Can2004.send0x122(); });
  controller.add(&Can2004send0x122);
  Can2004send0x169.enabled = true;
  Can2004send0x169.setInterval(1000);
  Can2004send0x169.onRun([&Can2004] { Can2004.send0x169(); });
  controller.add(&Can2004send0x169);
  Can2004send0x236.enabled = true;
  Can2004send0x236.setInterval(500);
  Can2004send0x236.onRun([&Can2004] { Can2004.send0x236(); });
  controller.add(&Can2004send0x236);
  Can2004send0x276.enabled = true;
  Can2004send0x276.setInterval(1000);
  Can2004send0x276.onRun([&Can2004] { Can2004.send0x276(); });
  controller.add(&Can2004send0x276);
  Can2004send0x350.enabled = true;
  Can2004send0x350.setInterval(500);
  Can2004send0x350.onRun([&Can2004] { Can2004.send0x350(); });
  controller.add(&Can2004send0x350);

  Can2010send0x228.enabled = true;
  Can2010send0x228.setInterval(60000);
  Can2010send0x228.onRun([&Can2010] { Can2010.send0x228(); });
  controller.add(&Can2010send0x228);
  Can2010send0x228.run();
  Can2010send0x3f6.enabled = true;
  Can2010send0x3f6.setInterval(1000);
  Can2010send0x3f6.onRun([&Can2010] { Can2010.send0x3f6(); });
  controller.add(&Can2010send0x3f6);
}

void loop() {
  controller.run();
}
