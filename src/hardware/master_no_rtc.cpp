/* Hardware profile: master_no_rtc
 * - No RTC module
 * - Time is fetched from internet API
 */

#if defined(HARDWARE_MASTER_NO_RTC)

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <nixiDriver.h>
#include <clock_variant_config.h>

#ifndef WIFI_SSID
#define WIFI_SSID "YOUR_SSID"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "YOUR_PASSWORD"
#endif

#ifndef TIME_API_URL
#define TIME_API_URL "http://worldtimeapi.org/api/timezone/Europe/Berlin.json"
#endif

static const char *WORLD_TIME_URL = TIME_API_URL;

nixiDriver ClockDisplay(4, 5, 2, CLOCK_IS_NUMITRON);
HTTPClient http;
StaticJsonDocument<2048> doc;

unsigned long lastSyncMs = 0;
const unsigned long syncIntervalMs = 5000;

static bool connectWiFi()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    return true;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  for (int i = 0; i < 40; ++i)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(250);
  }

  return false;
}

static bool fetchAndShowTime()
{
  http.begin(WORLD_TIME_URL);
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

  ClockDisplay.writeSegment(date[11] - '0', SEGMENT_1);
  ClockDisplay.writeSegment(date[12] - '0', SEGMENT_2);
  ClockDisplay.writeSegment(date[14] - '0', SEGMENT_3);
  ClockDisplay.writeSegment(date[15] - '0', SEGMENT_4);
  return true;
}

void setup()
{
  Serial.begin(57600);
  ClockDisplay.off();
}

void loop()
{
  if (!connectWiFi())
  {
    delay(1000);
    return;
  }

  const unsigned long now = millis();
  if (now - lastSyncMs >= syncIntervalMs)
  {
    if (fetchAndShowTime())
    {
      lastSyncMs = now;
    }
  }

  delay(50);
}

#endif
