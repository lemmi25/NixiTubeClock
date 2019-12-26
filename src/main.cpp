/* Code written by Moritz Boesenberg 26/12/2019 under the MIT licence */

#include <nixiDriver.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>

StaticJsonDocument<500> doc;
StaticJsonDocument<5000> docWeather;

nixiDriver NixiClock(4, 5, 2);

HTTPClient http;
HTTPClient httpWeather;

char timeOld;
bool enableTimeOld = false;

//const char *ssid = "UPC3442387";
//const char *password = "Ufppvydmk8mw";

//const char *ssid = "MikroTik-9CBB75";
//const char *password = "";

//Set your WiFi Name (SSID) and Password here
const char *ssid = "FRITZ!Box 7412";
const char *password = "92051906009500296929";

void setTemp(int temperature, int forecastTime);
void setPressure(int pressure, int forecastTime);

const char *date;

void setup()
{

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

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
}

void loop()
{
  ArduinoOTA.handle();

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
      timeOld = date[15];
    }
    enableTimeOld = true;

    //Get Time
    date = doc["datetime"]; //Get current time

    NixiClock.writeSegment(date[11] - '0', 1);
    NixiClock.writeSegment(date[12] - '0', 2);

    NixiClock.writeSegment(date[14] - '0', 3);
    NixiClock.writeSegment(date[15] - '0', 4);

    vTaskDelay(2000); //2sec
  }

  else
  {
    Serial.println("Error on HTTP request Time");
  }

  //Temperature
  if (timeOld != date[15])
  {
    httpWeather.begin("http://api.openweathermap.org/data/2.5/forecast?q=Freiburg,de&cnt=2&units=metric&appid=03e2fbe874af4836c6bf932b697a809b");
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

      const int tempTime1 = docWeather["list"][0]["main"]["temp"];     //Get current time forecast
      const int pressure1 = docWeather["list"][0]["main"]["pressure"]; //Get current time forecast
      setTemp(tempTime1, 3);
      delay(4000); //4sec
      NixiClock.writeSegment(10, 1);
      NixiClock.writeSegment(10, 2);
      NixiClock.writeSegment(10, 3);
      NixiClock.writeSegment(10, 4);
      delay(500);
      setPressure(pressure1, 3);
      delay(4000); //4sec

      const int tempTime2 = docWeather["list"][1]["main"]["temp"];     //Get current time
      const int pressure2 = docWeather["list"][0]["main"]["pressure"]; //Get current time forecast
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
  if (temperature < 10 && temperature > 0)
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
