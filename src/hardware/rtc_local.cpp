/* Hardware profile: rtc_local
 * - RTC module present
 * - Time is read locally from RTC (no internet time sync)
 */

#if defined(HARDWARE_RTC_LOCAL)

#include <Arduino.h>
#include "RTClib.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <nixiDriver.h>
#include <clock_variant_config.h>
#include <clock_provisioning.h>

#ifndef WIFI_SYNC_SECONDS
#define WIFI_SYNC_SECONDS 3600
#endif

#ifndef MAX_SYNC_FAILS
#define MAX_SYNC_FAILS 3
#endif

RTC_DS1307 rtc;
nixiDriver ClockDisplay(4, 5, 2, CLOCK_IS_NUMITRON);
HTTPClient http;
StaticJsonDocument<2048> doc;
ClockConfig g_config;

unsigned long lastSyncMs = 0;
uint8_t syncFailures = 0;

const unsigned long syncIntervalMs = WIFI_SYNC_SECONDS * 1000UL;

static bool syncRtcFromInternet()
{
  if (!connectConfiguredWifi(g_config, 15000))
  {
    return false;
  }

  const String url = buildTimeApiUrlForCity(g_config.city);
  http.begin(url);
  const int httpCode = http.GET();

  if (httpCode <= 0)
  {
    http.end();
    return false;
  }

  const String payload = http.getString();
  http.end();

  DeserializationError error = deserializeJson(doc, payload);
  if (error)
  {
    return false;
  }

  const char *date = doc["datetime"];
  if (date == nullptr)
  {
    return false;
  }

  const uint16_t year = (date[0] - '0') * 1000 + (date[1] - '0') * 100 + (date[2] - '0') * 10 + (date[3] - '0');
  const uint8_t month = (date[5] - '0') * 10 + (date[6] - '0');
  const uint8_t day = (date[8] - '0') * 10 + (date[9] - '0');
  const uint8_t hour = (date[11] - '0') * 10 + (date[12] - '0');
  const uint8_t minute = (date[14] - '0') * 10 + (date[15] - '0');
  const uint8_t second = (date[17] - '0') * 10 + (date[18] - '0');

  rtc.adjust(DateTime(year, month, day, hour, minute, second));

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  return true;
}

void setup()
{
  Serial.begin(57600);
  delay(200);
  Serial.println("Boot profile: RTC_LOCAL");
  loadClockConfig(g_config);
  Serial.print("Configured city: ");
  Serial.println(g_config.city);

  if (shouldForceProvisioning())
  {
    Serial.println("FORCE_PROVISIONING enabled");
    runProvisioningPortalUntilConfigured();
  }

  if (!g_config.isValid())
  {
    Serial.println("Config invalid, opening provisioning AP");
    runProvisioningPortalUntilConfigured();
  }

  rtc.begin();

  if (!rtc.isrunning())
  {
    // Fallback only if RTC lost state.
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  if (syncRtcFromInternet())
  {
    lastSyncMs = millis();
  }
  else
  {
    Serial.println("Initial internet sync failed, opening provisioning AP");
    runProvisioningPortalUntilConfigured();
  }

  ClockDisplay.off();
}

void loop()
{
  const unsigned long nowMs = millis();
  if (nowMs - lastSyncMs >= syncIntervalMs)
  {
    if (syncRtcFromInternet())
    {
      lastSyncMs = nowMs;
      syncFailures = 0;
    }
    else
    {
      syncFailures++;
      if (syncFailures >= MAX_SYNC_FAILS)
      {
        runProvisioningPortalUntilConfigured();
      }
    }
  }

  DateTime now = rtc.now();

  ClockDisplay.writeSegment(now.hour() / 10, SEGMENT_1);
  ClockDisplay.writeSegment(now.hour() % 10, SEGMENT_2);
  ClockDisplay.writeSegment(now.minute() / 10, SEGMENT_3);
  ClockDisplay.writeSegment(now.minute() % 10, SEGMENT_4);

  delay(250);
}

#endif
