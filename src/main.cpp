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

unsigned int frequency = 1000;
unsigned int onDuration = 50;
unsigned int offDuration = 100;
unsigned int beeps = 2;
unsigned int pauseDuration = 500;
unsigned int cycles = 10;

HTTPClient http;
HTTPClient httpWeather;

char timeOld;
char timeCurrent;
bool enableTimeOld = false;

//PSWD setup

void taskOne(void *parameter);
void taskTwo(void *parameter);

void setup()
{

  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);

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
      taskOne,   /* Task function. */
      "TaskOne", /* String with name of task. */
      10000,     /* Stack size in bytes. */
      NULL,      /* Parameter passed as input of the task */
      2,         /* Priority of the task. */
      NULL);     /* Task handle. */

  xTaskCreate(
      taskTwo,   /* Task function. */
      "TaskOne", /* String with name of task. */
      10000,     /* Stack size in bytes. */
      NULL,      /* Parameter passed as input of the task */
      1,         /* Priority of the task. */
      NULL);     /* Task handle. */
}

void loop()
{
  delay(100);
}

void taskTwo(void *parameter)
{

  for (;;)
  {
    Serial.println("HIGH");
    digitalWrite(26, HIGH);
    digitalWrite(27, HIGH);
    vTaskDelay(10); //2sec
    Serial.println("LOW");
    digitalWrite(26, LOW);
    digitalWrite(27, LOW);
    vTaskDelay(10); //2sec

    DateTime time = rtc.now();

    NixiClock.writeSegment(1, 1);
    NixiClock.writeSegment(2, 2);
    NixiClock.writeSegment(3, 3);
    NixiClock.writeSegment(4, 4);
  }
}

void taskOne(void *parameter)
{

  for (;;)
  {
    ArduinoOTA.handle();
  }
}