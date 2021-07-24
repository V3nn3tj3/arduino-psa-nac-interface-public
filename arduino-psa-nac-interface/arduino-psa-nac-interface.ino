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

#include <Thread.h>
#include <StaticThreadController.h>
#include "configuration.h"
#include "can2004.h"
#include "can2010.h"
#if HasAnalogButtons
#include "input.h"
#endif
#include <TimeLib.h>
#include <DS3232RTC.h>

#if HasAnalogButtons
Thread* InputThreat = new Thread();
#endif
Thread* Can2004Threat = new Thread();
Thread* Can2010Threat = new Thread();

Thread* Can2004send0x122 = new Thread();
Thread* Can2004send0x169 = new Thread();
Thread* Can2004send0x236 = new Thread();
Thread* Can2004send0x276 = new Thread();
Thread* Can2004send0x361 = new Thread();

Thread* Can2010send0x167 = new Thread();
Thread* Can2010send0x228 = new Thread();
Thread* Can2010send0x3f6 = new Thread();
Thread* Can2010send0x525 = new Thread();
Thread* Can2010send0x15B = new Thread();

#if HasAnalogButtons
StaticThreadController<13> controller (InputThreat,
                                       Can2004Threat,
                                       Can2010Threat,
                                       Can2004send0x122,
                                       Can2004send0x169,
                                       Can2004send0x236,
                                       Can2004send0x276,
                                       Can2004send0x361,
                                       Can2010send0x167,
                                       Can2010send0x228,
                                       Can2010send0x3f6,
                                       Can2010send0x525,
                                       Can2010send0x15B);
#else
StaticThreadController<12> controller (Can2004Threat,
                                       Can2010Threat,
                                       Can2004send0x122,
                                       Can2004send0x169,
                                       Can2004send0x236,
                                       Can2004send0x276,
                                       Can2004send0x361,
                                       Can2010send0x167,
                                       Can2010send0x228,
                                       Can2010send0x3f6,
                                       Can2010send0x525,
                                       Can2010send0x15B);
#endif
void setup() {
#if SERIAL_ENABLED || SERIAL_ENABLED_CAN
  Serial.begin(SERIAL_SPEED);
#endif
  setSyncProvider(RTC.get);
  if (timeStatus() != timeSet) {
#if SERIAL_ENABLED
    Serial.println(F("Unable to sync with the RTC"));
#endif
    //set/sync time
    setTime(Configuration.getDateTime().Hour, Configuration.getDateTime().Minute, 0, Configuration.getDateTime().Day, Configuration.getDateTime().Month, Configuration.getDateTime().Year);
  }
#if SERIAL_ENABLED
  else {
    Serial.println(F("RTC has set the system time"));
  }
#endif

  delay(2000);

  Can2004.setupCan();
  Can2010.setupCan();

#if HasAnalogButtons
  InputThreat->enabled = true;
  InputThreat->onRun([] { Input.process(); });
#endif
  Can2004Threat->enabled = true;
  Can2004Threat->onRun([] { Can2004.receive(); });
  Can2010Threat->enabled = true;
  Can2010Threat->onRun([] { Can2010.receive(); });

  Can2004send0x122->enabled = true;
  Can2004send0x122->setInterval(200);
  Can2004send0x122->onRun([] { Can2004.send0x122(); });
  Can2004send0x169->enabled = true;
  Can2004send0x169->setInterval(1000);
  Can2004send0x169->onRun([] { Can2004.send0x169(); });
  Can2004send0x236->enabled = true;
  Can2004send0x236->setInterval(500);
  Can2004send0x236->onRun([] { Can2004.send0x236(); });
  Can2004send0x276->enabled = true;
  Can2004send0x276->setInterval(1000);
  Can2004send0x276->onRun([] { Can2004.send0x276(); });
  Can2004send0x361->enabled = true;
  Can2004send0x361->setInterval(500);
  Can2004send0x361->onRun([] { Can2004.send0x361(); });

  Can2010send0x167->enabled = true;
  Can2010send0x167->setInterval(100);
  Can2010send0x167->onRun([] { Can2010.send0x167(); });
  Can2010send0x525->enabled = true;
  Can2010send0x525->setInterval(1000);
  Can2010send0x525->onRun([] { Can2010.send0x525(); });
  Can2010send0x228->enabled = true;
  Can2010send0x228->setInterval(60000);
  Can2010send0x228->onRun([] { Can2010.send0x228(); });
  Can2010send0x228->run();
  Can2010send0x3f6->enabled = true;
  Can2010send0x3f6->setInterval(1000);
  Can2010send0x3f6->onRun([] { Can2010.send0x3f6(); });
  Can2010send0x15B->enabled = true;
  Can2010send0x15B->setInterval(500);
  Can2010send0x15B->onRun([] { Can2010.send0x15B(); });
  Can2010.setupVersion();
}

void loop() {
  controller.run();
}
