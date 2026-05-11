/* Hardware profile: master_no_rtc
 * - No RTC module
 * - Time is fetched from internet API
 * - FreeRTOS tasks: display/button (Core 1), WiFi sync + OTA (Core 0)
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

#ifndef OTA_PASSWORD
#define OTA_PASSWORD ""
#endif

#ifndef PROVISION_ON_CONNECT_FAIL
#define PROVISION_ON_CONNECT_FAIL 1
#endif

// ---------------------------------------------------------------------------
// Pin definitions
// ---------------------------------------------------------------------------
#define BTN_ON_PIN    3    // Power/sleep button (active LOW, shared with UART RX)
#define BTN_MODE_PIN  15   // Menu button (active LOW, interrupt-driven)
#define LED_RED_PIN   27
#define LED_WHITE_PIN 26
#define LED_RED_CH    0
#define LED_WHITE_CH  1
#define LED_PWM_FREQ  150000
#define LED_PWM_BITS  10
// Logical full-brightness target used by existing fade/write paths.
#define LED_MAX       1024
#define LED_DIM       70

// ---------------------------------------------------------------------------
// Globals shared between tasks (use volatile / SemaphoreHandle_t for safety)
// ---------------------------------------------------------------------------
nixiDriver ClockDisplay(4, 5, 2, CLOCK_IS_NUMITRON);
ClockConfig g_config;

// Time reference — written by task_wifi, read by task_display
static SemaphoreHandle_t g_timeMutex;
static volatile uint32_t g_syncedSecondsOfDay = 0;
static volatile unsigned long g_syncedMillis   = 0;
static volatile bool          g_hasTime        = false;

// OTA state — only touched by task_wifi
static bool otaInitialized  = false;

const unsigned long syncIntervalMs = WIFI_SYNC_SECONDS * 1000UL;

// Button mode — written by ISR, read by task_display
enum ClockMode { MODE_NORMAL, MODE_ON, MODE_OFF, MODE_FUNCTION };
static volatile ClockMode g_clockMode = MODE_ON;

void IRAM_ATTR isr_btnMode()
{
  g_clockMode = MODE_FUNCTION;
}

// ---------------------------------------------------------------------------
// Helper: LED fade
// ---------------------------------------------------------------------------
static void ledFade(int from, int to, int stepDelayMs)
{
  if (from <= to)
  {
    for (int i = from; i <= to; i++)
    {
      ledcWrite(LED_WHITE_CH, i);
      ledcWrite(LED_RED_CH,   i);
      vTaskDelay(pdMS_TO_TICKS(stepDelayMs));
    }
  }
  else
  {
    for (int i = from; i >= to; i--)
    {
      ledcWrite(LED_WHITE_CH, i);
      ledcWrite(LED_RED_CH,   i);
      vTaskDelay(pdMS_TO_TICKS(stepDelayMs));
    }
  }
}

// ---------------------------------------------------------------------------
// Helper: display
// ---------------------------------------------------------------------------
static void showHourMinute(uint8_t hour, uint8_t minute)
{
  ClockDisplay.writeSegment(hour   / 10, SEGMENT_1);
  ClockDisplay.writeSegment(hour   % 10, SEGMENT_2);
  ClockDisplay.writeSegment(minute / 10, SEGMENT_3);
  ClockDisplay.writeSegment(minute % 10, SEGMENT_4);
}

// ---------------------------------------------------------------------------
// Stopwatch (runs inside task_display)
// ---------------------------------------------------------------------------
static void runStopwatch()
{
  ledcWrite(LED_RED_CH, 0);
  int sec = 0, s = 0, m = 0;
  uint8_t togW = 1, togR = 0;
  int count = 0;

  for (;;)
  {
    if (m >= 60 || digitalRead(BTN_ON_PIN) == LOW)
    {
      g_clockMode = MODE_NORMAL;
      vTaskDelay(pdMS_TO_TICKS(700));
      break;
    }
    if (count % 100 == 0)
    {
      ledcWrite(LED_WHITE_CH, togR * LED_MAX);
      ledcWrite(LED_RED_CH,   togW * LED_MAX);
      m = sec / 60;
      s = sec - m * 60;
      ClockDisplay.writeSegment(m / 10, SEGMENT_1);
      ClockDisplay.writeSegment(m % 10, SEGMENT_2);
      ClockDisplay.writeSegment(s / 10, SEGMENT_3);
      ClockDisplay.writeSegment(s % 10, SEGMENT_4);
      sec++;
      togW = !togW;
      togR = !togR;
    }
    count++;
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// ---------------------------------------------------------------------------
// Sensor display (runs inside task_display, only when ENABLE_TEMP_SENSOR)
// ---------------------------------------------------------------------------
#if ENABLE_TEMP_SENSOR
static void showSensorDisplay()
{
  const float rawT = tempSensor.getTemperature();
  const float rawH = tempSensor.getHumidity();
  const int8_t  t  = (int8_t)(rawT >= 0 ? rawT + 0.5f : rawT - 0.5f);
  const uint8_t h  = (uint8_t)(rawH + 0.5f);
  const uint8_t at = (uint8_t)(t < 0 ? -t : t);

  ledcWrite(LED_WHITE_CH, LED_MAX);
  ledcWrite(LED_RED_CH, t < 0 ? 0 : LED_MAX);

  ClockDisplay.writeSegment(t < 0 ? 0 : 10, SEGMENT_1);
  ClockDisplay.writeSegment(t < 0 ? 0 : 10, SEGMENT_2);
  ClockDisplay.writeSegment(at / 10, SEGMENT_3);
  ClockDisplay.writeSegment(at % 10, SEGMENT_4);
  vTaskDelay(pdMS_TO_TICKS(1500));

  ClockDisplay.writeSegment(10,     SEGMENT_1);
  ClockDisplay.writeSegment(10,     SEGMENT_2);
  ClockDisplay.writeSegment(h / 10, SEGMENT_3);
  ClockDisplay.writeSegment(h % 10, SEGMENT_4);
  ledcWrite(LED_WHITE_CH, LED_MAX);
  ledcWrite(LED_RED_CH,   0);
  vTaskDelay(pdMS_TO_TICKS(1500));

  Serial.printf("[SENSOR] Temp: %d C  Humidity: %u%%\n", t, h);
  g_clockMode = MODE_NORMAL;
}
#endif

// ---------------------------------------------------------------------------
// OTA helpers (used by task_wifi)
// ---------------------------------------------------------------------------
static bool isOtaWindowActive()
{
#if ENABLE_OTA
  return otaInitialized;
#else
  return false;
#endif
}

static void setupOtaWindow()
{
#if ENABLE_OTA
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  if (strlen(OTA_PASSWORD) > 0)
    ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.onStart([]() { Serial.println("[OTA] Update started"); });
  ArduinoOTA.onEnd([]()   { Serial.println("[OTA] Update finished"); });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("[OTA] Error %d\n", (int)error);
  });
  ArduinoOTA.begin();
  otaInitialized = true;
  Serial.printf("[OTA] Ready at %s (always on)\n",
                WiFi.localIP().toString().c_str());
#endif
}

// ---------------------------------------------------------------------------
// WiFi sync (used by task_wifi)
// ---------------------------------------------------------------------------
static bool connectConfiguredWifiBlocking(uint32_t timeoutMs)
{
  if (WiFi.status() == WL_CONNECTED) return true;
  WiFi.mode(WIFI_STA);
  if (!applyStaNetworkConfig()) return false;
  WiFi.begin(g_config.ssid.c_str(), g_config.password.c_str());
  const unsigned long t0 = millis();
  while (millis() - t0 < timeoutMs)
  {
    if (WiFi.status() == WL_CONNECTED) return true;
    vTaskDelay(pdMS_TO_TICKS(250));
  }
  return false;
}

static bool syncFromInternet()
{
  WiFiClientSecure secureClient;
  secureClient.setInsecure();
  HTTPClient http;
  http.begin(secureClient, buildTimeApiUrlForCity(g_config.city));
  const int httpCode = http.GET();
  if (httpCode <= 0) { http.end(); return false; }

  StaticJsonDocument<2048> doc;
  DeserializationError err = deserializeJson(doc, http.getString());
  http.end();
  if (err) return false;

  const char *date = doc["datetime"] | doc["dateTime"] | (const char *)nullptr;
  if (!date) return false;

  const uint8_t hh = (date[11]-'0')*10 + (date[12]-'0');
  const uint8_t mm = (date[14]-'0')*10 + (date[15]-'0');
  const uint8_t ss = (date[17]-'0')*10 + (date[18]-'0');
  const uint32_t sod = (uint32_t)hh*3600UL + (uint32_t)mm*60UL + ss;

  if (xSemaphoreTake(g_timeMutex, pdMS_TO_TICKS(100)) == pdTRUE)
  {
    g_syncedSecondsOfDay = sod;
    g_syncedMillis       = millis();
    g_hasTime            = true;
    xSemaphoreGive(g_timeMutex);
  }

  Serial.printf("[SYNC] %s -> %02u:%02u:%02u\n", date, hh, mm, ss);
  return true;
}

// ---------------------------------------------------------------------------
// FreeRTOS Task: WiFi sync + OTA (Core 0)
// ---------------------------------------------------------------------------
static void task_wifi(void *)
{
  unsigned long lastSyncMs = 0;
  bool otaSetUp = false;

  for (;;)
  {
    const unsigned long now = millis();
    const bool needSync = !g_hasTime || (now - lastSyncMs >= syncIntervalMs);

    if (needSync)
    {
      const bool wifiConnected = connectConfiguredWifiBlocking(15000);

      if (!wifiConnected)
      {
        #if PROVISION_ON_CONNECT_FAIL
        Serial.println("[WIFI] Connect failed - opening provisioning portal");
        runProvisioningPortalUntilConfigured();
        #else
        Serial.println("[WIFI] Connect failed - provisioning on connect fail is disabled");
        #endif
      }
      else
      {
        if (!otaSetUp)
        {
          setupOtaWindow();
          otaSetUp = true;
        }

        if (syncFromInternet())
        {
          lastSyncMs = now;
        }
        else
        {
          Serial.println("[SYNC] Time API sync failed; keeping existing time and retrying later");
        }
      }
    }

#if ENABLE_OTA
    if (otaSetUp)
    {
      ArduinoOTA.handle();
    }
#endif

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// ---------------------------------------------------------------------------
// FreeRTOS Task: display + buttons (Core 1)
// ---------------------------------------------------------------------------
static void task_display(void *)
{
  static uint8_t lastPrintedMinute = 255;

  for (;;)
  {
    const ClockMode mode = g_clockMode;

    if (mode == MODE_OFF)
    {
      // Fade LEDs down, blank tubes, wait for BTN_ON
      ledFade(LED_DIM, 0, 5);
      ClockDisplay.off();
      while (digitalRead(BTN_ON_PIN) != LOW)
        vTaskDelay(pdMS_TO_TICKS(10));
      g_clockMode = MODE_ON;
      vTaskDelay(pdMS_TO_TICKS(500)); // debounce
    }
    else if (mode == MODE_ON)
    {
      // Wake: fade LEDs up
      ledFade(0, LED_MAX, 2);
      ledFade(LED_MAX, LED_DIM, 2);
      g_clockMode = MODE_NORMAL;
    }
    else if (mode == MODE_FUNCTION)
    {
      // Measure how long BTN_MODE is held
      uint32_t held = 0;
      while (digitalRead(BTN_MODE_PIN) == LOW)
      {
        held++;
        vTaskDelay(pdMS_TO_TICKS(10));
      }
      if (held >= 100) // >= 1 second: stopwatch
      {
        runStopwatch();
      }
      else             // short press: sensor display
      {
#if ENABLE_TEMP_SENSOR
        showSensorDisplay();
#else
        g_clockMode = MODE_NORMAL;
#endif
      }
    }
    else // MODE_NORMAL
    {
      // Sleep button check
      if (digitalRead(BTN_ON_PIN) == LOW)
      {
        g_clockMode = MODE_OFF;
        vTaskDelay(pdMS_TO_TICKS(20));
        continue;
      }

      ledcWrite(LED_RED_CH,   LED_DIM);
      ledcWrite(LED_WHITE_CH, LED_DIM);

      // Render clock from shared time reference
      if (xSemaphoreTake(g_timeMutex, pdMS_TO_TICKS(10)) == pdTRUE)
      {
        if (g_hasTime)
        {
          const uint32_t elapsed = (millis() - g_syncedMillis) / 1000UL;
          const uint32_t sod     = (g_syncedSecondsOfDay + elapsed) % 86400UL;
          const uint8_t  hour    = sod / 3600UL;
          const uint8_t  minute  = (sod % 3600UL) / 60UL;
          const uint8_t  second  = sod % 60UL;
          xSemaphoreGive(g_timeMutex);

          showHourMinute(hour, minute);

          if (minute != lastPrintedMinute)
          {
            lastPrintedMinute = minute;
            Serial.printf("[TIME] %02u:%02u:%02u\n", hour, minute, second);
          }
        }
        else
        {
          xSemaphoreGive(g_timeMutex);
        }
      }

      vTaskDelay(pdMS_TO_TICKS(200));
    }
  }
}

// ---------------------------------------------------------------------------
// setup() — runs on Core 1 before FreeRTOS scheduler
// ---------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  delay(3000);
  Serial.println("[BOOT] MASTER_NO_RTC starting");

  // Provisioning check (before touching GPIO4 / display)
  loadClockConfig(g_config);
  if (shouldForceProvisioning() || !g_config.isValid())
  {
    Serial.println("[BOOT] Opening provisioning portal");
    runProvisioningPortalUntilConfigured();
  }
  Serial.printf("[BOOT] City: %s\n", g_config.city.c_str());

  // Display — init after provisioning to avoid GPIO4/PSRAM conflict
  ClockDisplay.off();

  // Buttons
  // BTN_ON is shared with UART RX on this board, so keep it as plain INPUT.
  pinMode(BTN_ON_PIN,   INPUT);
  pinMode(BTN_MODE_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_MODE_PIN), isr_btnMode, FALLING);

  // LEDs — PWM setup then boot fade-in
  ledcSetup(LED_RED_CH,   LED_PWM_FREQ, LED_PWM_BITS);
  ledcSetup(LED_WHITE_CH, LED_PWM_FREQ, LED_PWM_BITS);
  ledcAttachPin(LED_RED_PIN,   LED_RED_CH);
  ledcAttachPin(LED_WHITE_PIN, LED_WHITE_CH);
  ledFade(0, LED_MAX, 2);   // ~2s fade-in
  ledFade(LED_MAX, LED_DIM, 2);

  g_timeMutex = xSemaphoreCreateMutex();

  // task_wifi on Core 0, task_display on Core 1
  xTaskCreatePinnedToCore(task_wifi,    "task_wifi",    8192, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(task_display, "task_display", 4096, NULL, 1, NULL, 1);

  Serial.println("[BOOT] Tasks started");
}

// loop() is empty — all work is done in FreeRTOS tasks
void loop()
{
  vTaskDelay(portMAX_DELAY);
}

#endif
