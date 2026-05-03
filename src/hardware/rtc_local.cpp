/* Hardware profile: rtc_local
 * - RTC module present
 * - Time is read locally from RTC (no internet time sync)
 */

#if defined(HARDWARE_RTC_LOCAL)

#include <Arduino.h>
#include "RTClib.h"
#include <nixiDriver.h>
#include <clock_variant_config.h>

RTC_DS1307 rtc;
nixiDriver ClockDisplay(4, 5, 2, CLOCK_IS_NUMITRON);

void setup()
{
  Serial.begin(57600);
  rtc.begin();

  if (!rtc.isrunning())
  {
    // Fallback only if RTC lost state.
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  ClockDisplay.off();
}

void loop()
{
  DateTime now = rtc.now();

  ClockDisplay.writeSegment(now.hour() / 10, SEGMENT_1);
  ClockDisplay.writeSegment(now.hour() % 10, SEGMENT_2);
  ClockDisplay.writeSegment(now.minute() / 10, SEGMENT_3);
  ClockDisplay.writeSegment(now.minute() % 10, SEGMENT_4);

  delay(250);
}

#endif
