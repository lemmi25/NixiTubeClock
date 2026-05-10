/* Hardware profile: rtc_local
 * - RTC module present
 * - Time is read locally from RTC (no internet time sync)
 */

#if defined(HARDWARE_RTC_LOCAL)

#include <Arduino.h>
#include "RTClib.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <nixiDriver.h>
#include <clock_variant_config.h>
#include <clock_provisioning.h>

#ifndef WIFI_SYNC_SECONDS
#define WIFI_SYNC_SECONDS 3600
#endif

#ifndef MAX_SYNC_FAILS
#define MAX_SYNC_FAILS 3
#endif

#ifndef ENABLE_OTA
#define ENABLE_OTA 0
#endif

#ifndef OTA_WINDOW_SECONDS
#define OTA_WINDOW_SECONDS 180
#endif

#ifndef OTA_HOSTNAME
#define OTA_HOSTNAME "NixiClock"
#endif

RTC_DS1307 rtc;
nixiDriver ClockDisplay(4, 5, 2, CLOCK_IS_NUMITRON);
HTTPClient http;
StaticJsonDocument<2048> doc;
ClockConfig g_config;

unsigned long lastSyncMs = 0;
uint8_t syncFailures = 0;
bool otaInitialized = false;
bool otaWindowClosed = false;
unsigned long otaStartMs = 0;

const unsigned long syncIntervalMs = WIFI_SYNC_SECONDS * 1000UL;
const unsigned long otaWindowMs = OTA_WINDOW_SECONDS * 1000UL;

static bool isOtaWindowActive()
{
#if ENABLE_OTA
  return otaInitialized && (millis() - otaStartMs < otaWindowMs);
#else
  return false;
#endif
}

static bool connectWifiForOta(uint32_t timeoutMs)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    return true;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(g_config.ssid.c_str(), g_config.password.c_str());

  const unsigned long started = millis();
  while (millis() - started < timeoutMs)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(250);
  }

  return false;
}

static void setupOtaWindow()
{
#if ENABLE_OTA
  if (!connectWifiForOta(15000))
  {
    Serial.println("OTA disabled for this boot: WiFi connect failed");
    return;
  }

  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.onStart([]() {
    Serial.println("OTA update started");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("OTA update finished");
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.print("OTA error: ");
    Serial.println((int)error);
  });
  ArduinoOTA.begin();
  otaInitialized = true;
  otaStartMs = millis();
  Serial.print("OTA ready at ");
  Serial.print(WiFi.localIP());
  Serial.print(" for ");
  Serial.print(OTA_WINDOW_SECONDS);
  Serial.println("s");
#endif
}

static void handleOta()
{
#if ENABLE_OTA
  if (!otaInitialized)
  {
    return;
  }

  if (isOtaWindowActive())
  {
    ArduinoOTA.handle();
    return;
  }

  if (!otaWindowClosed)
  {
    otaWindowClosed = true;
    Serial.println("OTA window closed; returning to normal sync behavior");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
  }
#endif
}

static bool syncRtcFromInternet()
{
  if (!connectConfiguredWifi(g_config, 15000))
  {
    return false;
  }

  const String url = buildTimeApiUrlForCity(g_config.city);
  WiFiClientSecure secureClient;
  secureClient.setInsecure();
  http.begin(secureClient, url);
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
    date = doc["dateTime"];
  }
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

  if (!isOtaWindowActive())
  {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
  }

  return true;
}

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(3000); // Wait for serial monitor to connect before printing boot messages
  Serial.println("Serial monitor connected (RTC_LOCAL)");
  Serial.println("Boot profile: RTC_LOCAL");

  loadClockConfig(g_config);
  Serial.print("Configured city: ");
  Serial.println(g_config.city);

  if (shouldForceProvisioning())
  {
    Serial.println("Provisioning portal: opening AP");
    runProvisioningPortalUntilConfigured();
  }

  if (!g_config.isValid())
  {
    Serial.println("Config invalid, opening provisioning AP");
    runProvisioningPortalUntilConfigured();
  }

  setupOtaWindow();

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

  // Init display after provisioning so GPIO4 (PSRAM CS) isn't toggled
  // while the provisioning web server is allocating heap memory.
  ClockDisplay.off();
}

void loop()
{
  const unsigned long nowMs = millis();
  handleOta();
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
