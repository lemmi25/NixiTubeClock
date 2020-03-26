/* Code written by Moritz Boesenberg 26/12/2019 under the MIT licence */

#include <nixiDriver.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include "RTClib.h"
#include "SHT21.h"
#include "EasyBuzzer.h"

RTC_DS1307 rtc;
SHT21 SHT21;

StaticJsonDocument<5000> doc;
StaticJsonDocument<5000> docWeather;

nixiDriver NixiClock(4, 5, 2);

unsigned int IN_4_SEGMENT_1 = 3;
unsigned int IN_4_SEGMENT_2 = 4;
unsigned int IN_4_SEGMENT_3 = 2;
unsigned int IN_4_SEGMENT_4 = 1;

unsigned int frequency = 1000;
unsigned int onDuration = 50;
unsigned int offDuration = 100;
unsigned int beeps = 2;
unsigned int pauseDuration = 500;
unsigned int cycles = 10;

unsigned int BTN_ON = 3;
unsigned int BTN_MODE = 15;
unsigned int LED_RED = 27;
unsigned int LED_WHITE = 26;

uint32_t state_count = 0;
uint32_t mode_count = 0;

enum Clock_state
{
  off,
  on,
  stop_clock,
  normal_clock
};

int state;

HTTPClient http;
HTTPClient httpWeather;

char timeOld;
char timeCurrent;
bool enableTimeOld = false;

//PSWD setup

void task_state(void *parameter);
void task_ota(void *parameter);
void stopwatch();
void IRAM_ATTR isr_mode();

void setup()
{

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_WHITE, OUTPUT);

  pinMode(BTN_MODE, INPUT);
  pinMode(BTN_ON, INPUT);

  attachInterrupt(BTN_MODE, isr_mode, FALLING);
  //pinMode(BTN_MODE, INPUT);

  EasyBuzzer.setPin(18);

  EasyBuzzer.beep(
      600,           // Frequency in hertz(HZ).
      onDuration,    // On Duration in milliseconds(ms).
      offDuration,   // Off Duration in milliseconds(ms).
      beeps,         // The number of beeps per cycle.
      pauseDuration, // Pause duration.
      cycles         // The number of cycle.
  );

  Serial.begin(57600);
  WiFi.mode(WIFI_STA);
  WiFi.begin("UPC3442387", "Ufppvydmk8mw");

  rtc.begin();
  SHT21.begin();

  if (!rtc.isrunning())
  {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("OTA ESP32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");
  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //NixiClock.bootUp(); //Show Segment from 0 to 9 with 500mil delay

  xTaskCreate(
      task_state, /* Task function. */
      "TaskOne",  /* String with name of task. */
      10000,      /* Stack size in bytes. */
      NULL,       /* Parameter passed as input of the task */
      1,          /* Priority of the task. */
      NULL);      /* Task handle. */

  xTaskCreate(
      task_ota,  /* Task function. */
      "TaskOne", /* String with name of task. */
      10000,     /* Stack size in bytes. */
      NULL,      /* Parameter passed as input of the task */
      1,         /* Priority of the task. */
      NULL);     /* Task handle. */
}

void loop()
{
  delay(50);
}

void task_state(void *parameter)
{

  for (;;)
  {
    delay(50);
    if (digitalRead(BTN_MODE) == 0)
    {
      stopwatch();
    }
    else if (digitalRead(BTN_ON) == 0)
    {
      if (state_count % 2 == 0)
      {
        state = off;
      }
      else
      {
        state = on;
      }

      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_WHITE, LOW);
      NixiClock.off();
      delay(500);

      state_count++;
    }
    else if (state == on || state == normal_clock)
    {
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_WHITE, HIGH);
      NixiClock.writeSegment(1, IN_4_SEGMENT_1);
      NixiClock.writeSegment(2, IN_4_SEGMENT_2);
      NixiClock.writeSegment(3, IN_4_SEGMENT_3);
      NixiClock.writeSegment(4, IN_4_SEGMENT_4);
    }
  }
}

void task_ota(void *parameter)
{

  for (;;)
  {
    ArduinoOTA.handle();
  }
}

void stopwatch()
{
  int sec = 0;
  int s = 0;
  int m = 0;

  digitalWrite(LED_WHITE, LOW);

  for (;;)
  {

    m = sec / 60;
    s = (sec - (m * 60));

    NixiClock.writeSegment(m / 10, IN_4_SEGMENT_1);
    NixiClock.writeSegment(m % 10, IN_4_SEGMENT_2);
    NixiClock.writeSegment(s / 10, IN_4_SEGMENT_3);
    NixiClock.writeSegment(s % 10, IN_4_SEGMENT_4);

    sec++;

    digitalWrite(LED_RED, !digitalRead(LED_RED));

    if (state == normal_clock || m == 60)
    {
      return;
    }

    delay(1000);
  }
}

volatile unsigned long oldTime = 0;

void IRAM_ATTR isr_mode()
{
  if ((millis() - oldTime) > 500)
  {
    state = stop_clock;
  }
  else
  {
    state = normal_clock;
  }

  oldTime = millis();
}