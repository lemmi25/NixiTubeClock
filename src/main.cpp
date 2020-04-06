/* Code written by Moritz Boesenberg 26/12/2019 under the MIT licence */

#include <nixiDriver.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include "RTClib.h"
#include "SHT21.h"
#include <string.h>
#include "EasyBuzzer.h"

//define the Nixi (ZM1000 or IN_4)
#define IN_4

RTC_DS1307 rtc;
SHT21 sht;

StaticJsonDocument<5000> doc;
StaticJsonDocument<5000> docWeather;

nixiDriver NixiClock(4, 5, 2);

#ifdef IN_4
unsigned int SEGMENT_1 = 3;
unsigned int SEGMENT_2 = 4;
unsigned int SEGMENT_3 = 2;
unsigned int SEGMENT_4 = 1;
#endif

#ifdef ZM1000
unsigned int SEGMENT_1 = 1;
unsigned int SEGMENT_2 = 2;
unsigned int SEGMENT_3 = 3;
unsigned int SEGMENT_4 = 4;
#endif

unsigned int BTN_ON = 3;
unsigned int BTN_MODE = 15;
unsigned int LED_RED = 27;
unsigned int LED_WHITE = 26;

unsigned int LED_WHITE_CHANNEL = 1;
unsigned int LED_RED_CHANNEL = 0;

uint8_t hour = 0;
uint8_t minute = 0;

int8_t temp = 0;
uint8_t humidity = 0;

bool weather_check_wifi = false;
int temp_wifi = 0;
int humidity_wifi = 0;

volatile unsigned long state_count = 0;

uint32_t mode_count = 0;

enum Clock_state
{
  off,
  on,
  function,
  normal_clock
};

int state;

HTTPClient http;
HTTPClient httpWeather;

char timeOld;
char timeCurrent;
bool enableTimeOld = false;

unsigned int count_on = 0;

//PSWD setup

void task_state(void *parameter);
void task_ota(void *parameter);
void task_wlan(void *parameter);
char *substring(const char *str, size_t begin, size_t len);
void stopwatch();
void sht21();
void offwatch();
void normalwatch();
void IRAM_ATTR isr_mode();

unsigned int frequency = 1000;
unsigned int duration = 1000;

// setting PWM properties
const int freq = 15000;
const int resolution = 10;
void setup()
{

  Serial.begin(57600);

  rtc.begin();
  sht.begin();

  if (!rtc.isrunning())
  {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_WHITE, OUTPUT);

  ledcSetup(LED_WHITE_CHANNEL, freq, resolution);
  ledcSetup(LED_RED_CHANNEL, freq, resolution);

  ledcAttachPin(LED_RED, LED_RED_CHANNEL);
  ledcAttachPin(LED_WHITE, LED_WHITE_CHANNEL);

  pinMode(BTN_MODE, INPUT);
  pinMode(BTN_ON, INPUT);

  attachInterrupt(BTN_MODE, isr_mode, FALLING);
  //pinMode(BTN_MODE, INPUT);

  EasyBuzzer.setPin(18);

  EasyBuzzer.singleBeep(
      frequency, // Frequency in hertz(HZ).
      duration   // Duration of the beep in milliseconds(ms).
  );

  WiFi.mode(WIFI_STA);
  WiFi.begin("UPC3442387", "Ufppvydmk8mw");

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

  NixiClock.bootUp(); //Show Segment from 0 to 9 with 500mil delay
  NixiClock.off();
  delay(1000);

  xTaskCreate(
      task_state,  /* Task function. */
      "TaskState", /* String with name of task. */
      10000,       /* Stack size in bytes. */
      NULL,        /* Parameter passed as input of the task */
      1,           /* Priority of the task. */
      NULL);       /* Task handle. */

  xTaskCreate(
      task_ota,  /* Task function. */
      "TaskOTA", /* String with name of task. */
      10000,     /* Stack size in bytes. */
      NULL,      /* Parameter passed as input of the task */
      1,         /* Priority of the task. */
      NULL);     /* Task handle. */

  xTaskCreate(
      task_wlan,  /* Task function. */
      "TaskWLAN", /* String with name of task. */
      10000,      /* Stack size in bytes. */
      NULL,       /* Parameter passed as input of the task */
      1,          /* Priority of the task. */
      NULL);      /* Task handle. */
}

void loop()
{
  delay(1000);

  //Get Sensor Data RTC
  DateTime now = rtc.now();
  hour = now.hour();
  minute = now.minute();

  //Get Sensor Data SHT21
  humidity = round(sht.getHumidity());
  temp = round(sht.getTemperature());
}

void task_state(void *parameter)
{

  state = on;

  for (;;)
  {
    delay(50);
    if (state == function)
    {
      //ISR is called
      uint32_t count_int = 0;
      for (;;)
      {
        if (digitalRead(BTN_MODE) == 0)
        {
          count_int++;
        }
        else
        {
          break;
        }

        delay(10);
      }

      for (;;)
      {
        if (count_int >= 100)
        {
          stopwatch();
          break;
        }
        else
        {
          sht21();
          break;
        }

        delay(10);
      }
    }
    else if (state == off)
    {
      offwatch();
    }
    else if (state == on || state == normal_clock)
    {
      normalwatch();
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

void task_wlan(void *parameter)
{
  for (;;)
  {
    http.begin("http://worldtimeapi.org/api/timezone/Europe/Berlin.json"); //Specify the URL
    int httpCodeTime = http.GET();

    if (httpCodeTime > 0)
    { //Check for the returning code

      // Deserialize the JSON document
      DeserializationError error = deserializeJson(doc, http.getString());

      if (error)
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        vTaskDelay(2000); //2sec
      }
      else
      {
        const char *date = doc["datetime"]; //Get current time

        size_t begin = 11;
        size_t end = 12;

        String houre = (String)substring(date, begin, end);

        size_t begin_1 = 14;
        size_t end_1 = 15;

        String minute = (String)substring(date, begin_1, end_1);

        rtc.adjust(DateTime(0, 0, 0, houre.toInt(), minute.toInt(), 0));

        delay(500); //0.5sec
      }
    }
    else
    {
      Serial.println("Error on HTTP request Time");
    }

    httpWeather.begin("http://api.openweathermap.org/data/2.5/forecast?q=Freiburg,de&cnt=3&units=metric&appid=03e2fbe874af4836c6bf932b697a809b");
    int httpCodeWeather = httpWeather.GET();
    weather_check_wifi = false;

    if (httpCodeWeather > 0)
    { //Check for the returning code

      DeserializationError errorWeather = deserializeJson(docWeather, httpWeather.getString());

      if (errorWeather)
      {
        Serial.print(F("deserializeJson() from weather failed: "));
        Serial.println(errorWeather.c_str());
        vTaskDelay(2000); //2sec
      }
      else
      {

        weather_check_wifi = true;
        const int tempwifi = docWeather["list"][0]["main"]["temp"];         //Get current time forecast 0h
        const int humiditywifi = docWeather["list"][0]["main"]["humidity"]; //Get current humidity forecast 0h

        temp_wifi = tempwifi;
        humidity_wifi = humiditywifi;

        delay(500); //0.5sec
      }
    }
    vTaskDelay(30000); //30sec
  }
}

char *substring(const char *str, size_t begin, size_t len)
{
  if (str == 0 || strlen(str) == 0 || strlen(str) < begin || strlen(str) < (begin + len))
    return 0;

  return strndup(str + begin, len);
}

void offwatch()
{

  for (int i = 70; i >= 0; --i)
  {
    ledcWrite(LED_RED_CHANNEL, i);
    ledcWrite(LED_WHITE_CHANNEL, i);
    delay(5);
  }

  NixiClock.off();
  delay(1000);

  noInterrupts();
  for (;;)
  {
    if (digitalRead(BTN_ON) == 0)
    {
      state = on;
      delay(500);
      return;
    }
    delay(10);
  }
  interrupts();
}

void normalwatch()
{
  if (digitalRead(BTN_ON) == 0)
  {
    state = off;
  }

  if (state == on)
  {
    for (int i = 0; i <= 1024; i++)
    {
      ledcWrite(LED_WHITE_CHANNEL, i);
      ledcWrite(LED_RED_CHANNEL, i);
      delay(1);
    }

    delay(20);
    NixiClock.writeSegment(hour / 10, SEGMENT_1);
    delay(5);
    NixiClock.writeSegment(hour % 10, SEGMENT_2);
    delay(5);
    NixiClock.writeSegment(minute / 10, SEGMENT_3);
    delay(5);
    NixiClock.writeSegment(minute % 10, SEGMENT_4);
    delay(20);

    for (int i = 1024; i >= 70; --i)
    {
      ledcWrite(LED_RED_CHANNEL, i);
      ledcWrite(LED_WHITE_CHANNEL, i);
      delay(4);
    }
    state = normal_clock;
  }

  if (count_on % 10 == 0)
  {
    NixiClock.writeSegment(hour / 10, SEGMENT_1);
    NixiClock.writeSegment(hour % 10, SEGMENT_2);
    NixiClock.writeSegment(minute / 10, SEGMENT_3);
    NixiClock.writeSegment(minute % 10, SEGMENT_4);
  }

  ledcWrite(LED_RED_CHANNEL, 70);
  ledcWrite(LED_WHITE_CHANNEL, 70);

  count_on++;
}

void stopwatch()
{
  ledcWrite(LED_RED_CHANNEL, 0);
  delay(10);

  int sec = 0;
  int s = 0;
  int m = 0;

  uint8_t toggle_led_white = 1;
  uint8_t toggle_led_red = 0;

  int count = 0;

  for (;;)
  {
    if (m == 60 || digitalRead(BTN_ON) == 0)
    {
      state = normal_clock;
      delay(700);
      return;
    }

    if (count % 100 == 0)
    {
      ledcWrite(LED_WHITE_CHANNEL, toggle_led_red * 1024);
      ledcWrite(LED_RED_CHANNEL, toggle_led_white * 1024);

      m = sec / 60;
      s = (sec - (m * 60));
      NixiClock.writeSegment(m / 10, SEGMENT_1);
      NixiClock.writeSegment(m % 10, SEGMENT_2);
      NixiClock.writeSegment(s / 10, SEGMENT_3);
      NixiClock.writeSegment(s % 10, SEGMENT_4);

      sec++;
      toggle_led_white = !toggle_led_white;
      toggle_led_red = !toggle_led_red;
    }
    count++;
    delay(10);
  }
}

void sht21()
{

  if (temp <= 0)
  {
    temp = (int8_t)abs(temp);

    NixiClock.writeSegment(0, SEGMENT_3);
    NixiClock.writeSegment(0, SEGMENT_4);
    NixiClock.writeSegment(temp / 10, SEGMENT_1);
    NixiClock.writeSegment(temp % 10, SEGMENT_2);
    ledcWrite(LED_RED_CHANNEL, 0);
    ledcWrite(LED_WHITE_CHANNEL, 1024);
  }
  else
  {
    NixiClock.writeSegment(10, SEGMENT_3);
    NixiClock.writeSegment(10, SEGMENT_4);
    NixiClock.writeSegment(temp / 10, SEGMENT_1);
    NixiClock.writeSegment(temp % 10, SEGMENT_2);
  }

  if (weather_check_wifi == true)
  {
    delay(1000);
    if (temp_wifi <= 0)
    {
      temp_wifi = (int8_t)abs(temp_wifi);

      NixiClock.writeSegment(temp_wifi / 10, SEGMENT_3);
      NixiClock.writeSegment(temp_wifi % 10, SEGMENT_4);

      ledcWrite(LED_RED_CHANNEL, 0);
      ledcWrite(LED_WHITE_CHANNEL, 1024);
    }
    else
    {
      NixiClock.writeSegment(temp_wifi / 10, SEGMENT_3);
      NixiClock.writeSegment(temp_wifi % 10, SEGMENT_4);

      ledcWrite(LED_WHITE_CHANNEL, 0);
      ledcWrite(LED_RED_CHANNEL, 1024);
    }
  }

  delay(1500);

  NixiClock.writeSegment(10, SEGMENT_3);
  NixiClock.writeSegment(10, SEGMENT_4);
  NixiClock.writeSegment(humidity / 10, SEGMENT_1);
  NixiClock.writeSegment(humidity % 10, SEGMENT_2);

  if (weather_check_wifi == true)
  {
    delay(1500);
    NixiClock.writeSegment(humidity_wifi / 10, SEGMENT_3);
    NixiClock.writeSegment(humidity_wifi % 10, SEGMENT_4);
  }

  ledcWrite(LED_WHITE_CHANNEL, 1024);
  ledcWrite(LED_RED_CHANNEL, 0);

  delay(1500);

  state = normal_clock;
}

void IRAM_ATTR isr_mode()
{
  state = function;
}