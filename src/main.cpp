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

void setTemp(int temperature, int forecastTime);
void setPressure(int pressure, int forecastTime);

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
}

void loop()
{
  EasyBuzzer.update();

  ArduinoOTA.handle();

  Serial.println("HIGH");
  digitalWrite(26, HIGH);
  digitalWrite(27, HIGH);
  vTaskDelay(5000); //2sec
  Serial.println("LOW");
  digitalWrite(26, LOW);
  digitalWrite(27, LOW);
  vTaskDelay(5000); //2sec

  DateTime time = rtc.now();

  if (rtc.isrunning() != 0)
  {
    //Full Timestamp
    Serial.println(String("DateTime::TIMESTAMP_FULL:\t") + time.timestamp(DateTime::TIMESTAMP_FULL));

    //Date Only
    Serial.println(String("DateTime::TIMESTAMP_DATE:\t") + time.timestamp(DateTime::TIMESTAMP_DATE));

    //Full Timestamp
    Serial.println(String("DateTime::TIMESTAMP_TIME:\t") + time.timestamp(DateTime::TIMESTAMP_TIME));

    Serial.println("\n");

    Serial.print("Humidity(%RH): ");
    Serial.print(SHT21.getHumidity());
    Serial.print("     Temperature(C): ");
    Serial.println(SHT21.getTemperature());
  }

  //Time

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
      return;
    }

    if (enableTimeOld == true)
    {
      timeOld = timeCurrent;
    }
    enableTimeOld = true;

    //Get Time
    const char *date = doc["datetime"]; //Get current time

    NixiClock.writeSegment(date[11] - '0', 1);
    NixiClock.writeSegment(date[12] - '0', 2);

    NixiClock.writeSegment(date[14] - '0', 3);
    NixiClock.writeSegment(date[15] - '0', 4);

    timeCurrent = date[15];

    Serial.println(date);

    vTaskDelay(2000); //2sec
  }

  else
  {
    Serial.println("Error on HTTP request Time");
  }

  //Temperature
  if (timeOld != timeCurrent)
  {
    httpWeather.begin("http://api.openweathermap.org/data/2.5/forecast?q=Velber,de&cnt=3&units=metric&appid=03e2fbe874af4836c6bf932b697a809b");
    int httpCodeWeather = httpWeather.GET();

    if (httpCodeWeather > 0)
    { //Check for the returning code

      DeserializationError errorWeather = deserializeJson(docWeather, httpWeather.getString());

      if (errorWeather)
      {
        Serial.print(F("deserializeJson() from weather failed: "));
        Serial.println(errorWeather.c_str());
        vTaskDelay(2000); //2sec
      }

      const int tempTime1 = docWeather["list"][1]["main"]["temp"];     //Get current time forecast 3h
      const int pressure1 = docWeather["list"][1]["main"]["pressure"]; //Get current pressure forecast 3h
      setTemp(tempTime1, 3);
      delay(4000); //4sec
      NixiClock.writeSegment(10, 1);
      NixiClock.writeSegment(10, 2);
      NixiClock.writeSegment(10, 3);
      NixiClock.writeSegment(10, 4);
      delay(500);
      setPressure(pressure1, 3);
      delay(4000); //4sec

      const int tempTime2 = docWeather["list"][2]["main"]["temp"];     //Get current time 6h
      const int pressure2 = docWeather["list"][2]["main"]["pressure"]; //Get pressure time forecast 6h
      setTemp(tempTime2, 6);
      delay(4000); //4sec
      NixiClock.writeSegment(10, 1);
      NixiClock.writeSegment(10, 2);
      NixiClock.writeSegment(10, 3);
      NixiClock.writeSegment(10, 4);
      delay(500);
      setPressure(pressure2, 3);
      delay(4000); //4sec
    }
    else
    {
      Serial.println("Error on HTTP request Date");
    }
  }
  else
  {
    return;
  }
}

int getdigit(int num, int n)
{
  int r, t1, t2;

  t1 = pow(10, n + 1);
  r = num % t1;

  if (n > 0)
  {
    t2 = pow(10, n);
    r = r / t2;
  }

  return r;
}

void setPressure(int pressure, int forecastTime)
{
  NixiClock.writeSegment(getdigit(pressure, 3), 1);
  NixiClock.writeSegment(getdigit(pressure, 2), 2);
  NixiClock.writeSegment(getdigit(pressure, 1), 3);
  NixiClock.writeSegment(getdigit(pressure, 0), 4);
}

void setTemp(int temperature, int forecastTime)
{
  if (temperature < 10 && temperature >= 0)
  {
    NixiClock.writeSegment(0, 1);
    NixiClock.writeSegment(forecastTime, 2);
    NixiClock.writeSegment(0, 3);
    NixiClock.writeSegment(temperature, 4);
    //Serial.println("Between 0 and 10");
  }
  else if (temperature > 9)
  {
    NixiClock.writeSegment(0, 1);
    NixiClock.writeSegment(forecastTime, 2);
    NixiClock.writeSegment(getdigit(temperature, 1), 3);
    NixiClock.writeSegment(getdigit(temperature, 0), 4);
    //Serial.println("above 10");
  }
  else if (temperature > -10 && temperature < 0)
  {
    NixiClock.writeSegment(1, 1);
    NixiClock.writeSegment(forecastTime, 2);
    NixiClock.writeSegment(0, 3);
    NixiClock.writeSegment(abs(temperature), 4);
    //Serial.println("between -10 and 0");
  }
  else if (temperature < -9)
  {
    NixiClock.writeSegment(1, 1);
    NixiClock.writeSegment(forecastTime, 2);
    NixiClock.writeSegment(getdigit(abs(temperature), 1), 3);
    NixiClock.writeSegment(getdigit(abs(temperature), 0), 4);
    //Serial.println("below -9");
  }
  else
  {
    return;
  }
}
