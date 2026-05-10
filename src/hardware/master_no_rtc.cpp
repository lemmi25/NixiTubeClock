/* Hardware profile: master_no_rtc
 * - No RTC module
 * - Time is fetched from internet API
 */

#if defined(HARDWARE_MASTER_NO_RTC)

#include <Arduino.h>
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

nixiDriver ClockDisplay(4, 5, 2, CLOCK_IS_NUMITRON);
HTTPClient http;
StaticJsonDocument<2048> doc;

ClockConfig g_config;

unsigned long lastSyncMs = 0;
unsigned long lastRenderMs = 0;

uint32_t syncedSecondsOfDay = 0;
unsigned long syncedMillis = 0;
bool hasTimeReference = false;
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

static uint32_t parseSecondsOfDay(const char *dateTime)
{
  const uint8_t hh = (dateTime[11] - '0') * 10 + (dateTime[12] - '0');
  const uint8_t mm = (dateTime[14] - '0') * 10 + (dateTime[15] - '0');
  const uint8_t ss = (dateTime[17] - '0') * 10 + (dateTime[18] - '0');
  return (uint32_t)hh * 3600UL + (uint32_t)mm * 60UL + (uint32_t)ss;
}

static void showHourMinute(uint8_t hour, uint8_t minute)
{
  ClockDisplay.writeSegment(hour / 10, SEGMENT_1);
  ClockDisplay.writeSegment(hour % 10, SEGMENT_2);
  ClockDisplay.writeSegment(minute / 10, SEGMENT_3);
  ClockDisplay.writeSegment(minute % 10, SEGMENT_4);
}

static bool syncFromInternet()
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

  syncedSecondsOfDay = parseSecondsOfDay(date);
  syncedMillis = millis();
  hasTimeReference = true;

  const uint8_t hour = syncedSecondsOfDay / 3600UL;
  const uint8_t minute = (syncedSecondsOfDay % 3600UL) / 60UL;
  showHourMinute(hour, minute);

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
  Serial.println("Serial monitor connected (MASTER_NO_RTC)");
  Serial.println("Boot profile: MASTER_NO_RTC");
  Serial.flush();

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

  // Init display after provisioning so GPIO4 (PSRAM CS) isn't toggled
  // while the provisioning web server is allocating heap memory.
  ClockDisplay.off();

  setupOtaWindow();
}

void loop()
{
  const unsigned long now = millis();
  handleOta();

  if (!hasTimeReference || (now - lastSyncMs >= syncIntervalMs))
  {
    if (syncFromInternet())
    {
      lastSyncMs = now;
      syncFailures = 0;
    }
    else
    {
      syncFailures++;
      if (!hasTimeReference)
      {
        Serial.println("Initial internet sync failed, opening provisioning AP");
        runProvisioningPortalUntilConfigured();
      }
      if (syncFailures >= MAX_SYNC_FAILS)
      {
        Serial.println("Max sync failures reached, opening provisioning AP");
        runProvisioningPortalUntilConfigured();
      }
    }
  }

  if (hasTimeReference && (now - lastRenderMs >= 200))
  {
    const uint32_t elapsed = (now - syncedMillis) / 1000UL;
    const uint32_t secondsOfDay = (syncedSecondsOfDay + elapsed) % 86400UL;
    const uint8_t hour = secondsOfDay / 3600UL;
    const uint8_t minute = (secondsOfDay % 3600UL) / 60UL;
    const uint8_t second = secondsOfDay % 60UL;
    showHourMinute(hour, minute);
    // Print time to serial every second
    static uint8_t lastSecond = 255;
    if (second != lastSecond)
    {
      lastSecond = second;
      Serial.printf("[TIME] %02u:%02u:%02u\n", hour, minute, second);
    }
    lastRenderMs = now;
  }

  delay(20);
}

#endif
