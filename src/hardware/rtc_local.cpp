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

#ifndef ENABLE_TEMP_SENSOR
#define ENABLE_TEMP_SENSOR 0
#endif

#if ENABLE_TEMP_SENSOR
#include <SHT21.h>
static SHT21 tempSensor;
#endif

#ifndef WIFI_SYNC_SECONDS
#define WIFI_SYNC_SECONDS 3600
#endif

#ifndef ENABLE_OTA
#define ENABLE_OTA 0
#endif

#ifndef OTA_HOSTNAME
#define OTA_HOSTNAME "NixiClock"
#endif

#ifndef PROVISION_ON_CONNECT_FAIL
#define PROVISION_ON_CONNECT_FAIL 1
#endif

#define BTN_ON_PIN 3  // Power/sleep button (active LOW, shared with UART RX)
#define BTN_MODE_PIN 15 // Menu button (active LOW, pull-up)
#define LED_RED_PIN   27
#define LED_WHITE_PIN 26
#define LED_RED_CH    0
#define LED_WHITE_CH  1
#define LED_PWM_FREQ  150000
#define LED_PWM_BITS  10
// 10-bit PWM full-scale duty.
#define LED_MAX       1023
#define LED_DIM       70


RTC_DS1307 rtc;
nixiDriver ClockDisplay(4, 5, 2, CLOCK_IS_NUMITRON);
HTTPClient http;
StaticJsonDocument<2048> doc;
ClockConfig g_config;

unsigned long lastSyncMs = 0;
bool otaInitialized = false;

const unsigned long syncIntervalMs = WIFI_SYNC_SECONDS * 1000UL;

enum PowerState { POWER_AWAKE, POWER_HIBERNATE };
static PowerState g_powerState = POWER_AWAKE;

static void ledFade(int from, int to, int stepDelayMs)
{
  if (from <= to)
  {
    for (int i = from; i <= to; i++)
    {
      ledcWrite(LED_WHITE_CH, i);
      ledcWrite(LED_RED_CH, i);
      delay(stepDelayMs);
    }
  }
  else
  {
    for (int i = from; i >= to; i--)
    {
      ledcWrite(LED_WHITE_CH, i);
      ledcWrite(LED_RED_CH, i);
      delay(stepDelayMs);
    }
  }
}

static void showHourMinute(uint8_t hour, uint8_t minute)
{
  ClockDisplay.writeSegment(hour / 10, SEGMENT_1);
  ClockDisplay.writeSegment(hour % 10, SEGMENT_2);
  ClockDisplay.writeSegment(minute / 10, SEGMENT_3);
  ClockDisplay.writeSegment(minute % 10, SEGMENT_4);
}

static void runStopwatch()
{
  uint32_t sec = 0;
  unsigned long lastTickMs = millis();
  // When false, stopwatch value is frozen on the display until power exits.
  bool stopwatchRunning = true;

  for (;;)
  {
    const unsigned long nowMs = millis();
    const bool modePressed = (digitalRead(BTN_MODE_PIN) == LOW);
    const bool powerPressed = (digitalRead(BTN_ON_PIN) == LOW);

    const uint8_t m = (uint8_t)(sec / 60U);
    const uint8_t s = (uint8_t)(sec % 60U);
    if (m >= 60)
    {
      break;
    }

    showHourMinute(m, s);

    // Menu press freezes stopwatch at the current value.
    if (modePressed && stopwatchRunning)
    {
      delay(20);
      if (digitalRead(BTN_MODE_PIN) == LOW)
      {
        // Menu press: stop stopwatch and keep showing the stopped time.
        while (digitalRead(BTN_MODE_PIN) == LOW)
        {
          delay(20);
        }
        stopwatchRunning = false;
      }
    }

    // Power press leaves stopwatch and returns to normal clock mode.
    if (powerPressed)
    {
      delay(20);
      if (digitalRead(BTN_ON_PIN) == LOW)
      {
        // Power press: leave stopwatch and return to normal clock time.
        while (digitalRead(BTN_ON_PIN) == LOW)
        {
          delay(20);
        }
        return;
      }
    }

    if (stopwatchRunning && nowMs - lastTickMs >= 1000)
    {
      lastTickMs += 1000;
      sec++;
    }

    delay(20);
  }
}

#if ENABLE_TEMP_SENSOR
static void showSensorDisplay()
{
  const float rawT = tempSensor.getTemperature();
  const float rawH = tempSensor.getHumidity();
  const int8_t t = (int8_t)(rawT >= 0 ? rawT + 0.5f : rawT - 0.5f);
  const uint8_t h = (uint8_t)(rawH + 0.5f);
  const uint8_t at = (uint8_t)(t < 0 ? -t : t);

  ClockDisplay.writeSegment(t < 0 ? 0 : 10, SEGMENT_1);
  ClockDisplay.writeSegment(t < 0 ? 0 : 10, SEGMENT_2);
  ClockDisplay.writeSegment(at / 10, SEGMENT_3);
  ClockDisplay.writeSegment(at % 10, SEGMENT_4);
  delay(1500);

  ClockDisplay.writeSegment(10, SEGMENT_1);
  ClockDisplay.writeSegment(10, SEGMENT_2);
  ClockDisplay.writeSegment(h / 10, SEGMENT_3);
  ClockDisplay.writeSegment(h % 10, SEGMENT_4);
  delay(1500);

  Serial.printf("[SENSOR] Temp: %d C  Humidity: %u%%\n", t, h);
}
#endif

static bool isOtaWindowActive()
{
#if ENABLE_OTA
  return otaInitialized;
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
  if (!applyStaNetworkConfig())
  {
    return false;
  }
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
  Serial.print("OTA ready at ");
  Serial.print(WiFi.localIP());
  Serial.println(" (always on)");
#endif
}

static void handleOta()
{
#if ENABLE_OTA
  if (!otaInitialized)
  {
    return;
  }

  ArduinoOTA.handle();
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

  Serial.printf("[SYNC] API time: %04u-%02u-%02u %02u:%02u:%02u -> RTC updated\n",
                year, month, day, hour, minute, second);

#if ENABLE_TEMP_SENSOR
  float tempC = tempSensor.getTemperature();
  float humidity = tempSensor.getHumidity();
  Serial.printf("[SENSOR] Temp: %.1f C  Humidity: %.1f%%\n", tempC, humidity);
#endif

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
    // Blocks in captive portal loop until config is applied (device reboots).
    runProvisioningPortalUntilConfigured();
  }

  // Reload config in case portal updated values before reboot/return path.
  loadClockConfig(g_config);

  // Placeholders/empty timezone should never proceed to normal runtime.
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
    if (WiFi.status() != WL_CONNECTED)
    {
#if PROVISION_ON_CONNECT_FAIL
      Serial.println("Initial WiFi connection failed, opening provisioning AP");
      runProvisioningPortalUntilConfigured();
#else
      Serial.println("Initial WiFi connection failed; provisioning on connect fail is disabled");
#endif
    }
    else
    {
      Serial.println("Initial internet sync failed (API), keeping saved WiFi config");
    }
  }

  // Init display after provisioning so GPIO4 (PSRAM CS) isn't toggled
  // while the provisioning web server is allocating heap memory.
  ClockDisplay.off();

  // BTN_ON is active LOW; use floating input because this line is shared with UART RX.
  pinMode(BTN_ON_PIN, INPUT);
  pinMode(BTN_MODE_PIN, INPUT_PULLUP);

  // LEDs — match RTOS profile boot behavior
  ledcSetup(LED_RED_CH, LED_PWM_FREQ, LED_PWM_BITS);
  ledcSetup(LED_WHITE_CH, LED_PWM_FREQ, LED_PWM_BITS);
  ledcAttachPin(LED_RED_PIN, LED_RED_CH);
  ledcAttachPin(LED_WHITE_PIN, LED_WHITE_CH);
  ledcWrite(LED_WHITE_CH, LED_MAX);
  ledcWrite(LED_RED_CH, LED_MAX);
  delay(3000);
  ledFade(LED_MAX, LED_DIM, 6);
}

void loop()
{
  const unsigned long nowMs = millis();

  // BTN_ON is on GPIO3 (shared with UART RX): avoid edge-only debounce logic,
  // use confirmed raw level + hold tracking for robust behavior.
  static bool sleepHoldTracking = false;
  static unsigned long sleepHoldStartMs = 0;
  // Ensures the same press that caused sleep does not immediately wake again.
  static bool requireReleaseAfterSleep = false;

  const bool btnOnRaw = (digitalRead(BTN_ON_PIN) == LOW);

  // Awake state: require ~500ms hold to enter hibernate.
  if (g_powerState == POWER_AWAKE)
  {
    if (btnOnRaw)
    {
      if (!sleepHoldTracking)
      {
        delay(20);
        if (digitalRead(BTN_ON_PIN) == LOW)
        {
          sleepHoldTracking = true;
          sleepHoldStartMs = nowMs;
        }
      }
      else if (nowMs - sleepHoldStartMs >= 500)
      {
        g_powerState = POWER_HIBERNATE;
        ledFade(LED_DIM, 0, 1);
        ClockDisplay.off();
        ledcWrite(LED_WHITE_CH, 0);
        ledcWrite(LED_RED_CH, 0);
        requireReleaseAfterSleep = true;
        sleepHoldTracking = false;
        delay(20);
        return;
      }
    }
    else
    {
      sleepHoldTracking = false;
    }
  }

  if (g_powerState == POWER_HIBERNATE)
  {
    ClockDisplay.off();
    ledcWrite(LED_WHITE_CH, 0);
    ledcWrite(LED_RED_CH, 0);

    // Gate wake until button is released at least once after entering sleep.
    if (requireReleaseAfterSleep)
    {
      if (!btnOnRaw)
      {
        requireReleaseAfterSleep = false;
      }
      delay(20);
      return;
    }

    // Off state: short press wakes the clock (raw check + confirm for reliability).
    if (btnOnRaw)
    {
      delay(20);
      if (digitalRead(BTN_ON_PIN) != LOW)
      {
        return;
      }

      g_powerState = POWER_AWAKE;
      ledFade(0, LED_DIM, 1);
      delay(180);
    }

    delay(20);
    return;
  }

  // MENU button behavior:
  // short press (<1s): sensor display (if enabled)
  // long press (>=1s): stopwatch
  // held counts 10ms ticks, so held>=100 means about 1 second.
  if (digitalRead(BTN_MODE_PIN) == LOW)
  {
    unsigned long held = 0;
    while (digitalRead(BTN_MODE_PIN) == LOW)
    {
      held++;
      delay(10);
    }

    if (held >= 100)
    {
      runStopwatch();
    }
    else
    {
#if ENABLE_TEMP_SENSOR
      showSensorDisplay();
#endif
    }
  }

  handleOta();
  if (nowMs - lastSyncMs >= syncIntervalMs)
  {
    if (syncRtcFromInternet())
    {
      lastSyncMs = nowMs;
    }
    else
    {
      if (WiFi.status() != WL_CONNECTED)
      {
        // Reboot-time connection failure handles provisioning fallback.
        // During runtime, keep retrying silently to avoid reopening AP.
        Serial.println("WiFi disconnected during runtime; retrying without opening provisioning AP");
      }
      else
      {
        // API/server issue while WiFi is connected - keep running and retry later.
        Serial.println("Internet time sync failed (API), WiFi still connected");
      }
    }
  }

  DateTime now = rtc.now();

  ledcWrite(LED_WHITE_CH, LED_DIM);
  ledcWrite(LED_RED_CH, LED_DIM);

  static unsigned long lastDisplayUpdateMs = 0;
  if (nowMs - lastDisplayUpdateMs >= 200)
  {
    lastDisplayUpdateMs = nowMs;
    showHourMinute(now.hour(), now.minute());
  }

  // Print RTC time once per minute
  static uint8_t lastPrintedMinute = 255;
  if (now.minute() != lastPrintedMinute)
  {
    lastPrintedMinute = now.minute();
    Serial.printf("[RTC]  %02u:%02u:%02u\n", now.hour(), now.minute(), now.second());
  }

  delay(20);
}

#endif
