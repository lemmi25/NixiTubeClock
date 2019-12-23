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

int timeOld = 0;

//const char *ssid = "UPC3442387";
//const char *password = "Ufppvydmk8mw";

//const char *ssid = "MikroTik-9CBB75";
//const char *password = "";

const char *ssid = "WLAN-164097";
const char *password = "4028408165188671";

char houre1buff[2];
char houre2buff[2];
char minute1buff[2];
char minute2buff[2];

char forecastTime1buff;
char forecastTime2buff;

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

  //NixiClock.bootUp(); //Show Segment from 0 to 9 with 500mil delay
  //delay(2000);
}

void loop()
{
  ArduinoOTA.handle();

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
      delay(2000);
      return;
    }

    //Get Time
    const char *date = doc["datetime"]; //Get current time

    //Get Time
    /* JsonArray array = docWeather.as<JsonArray>();
    const char *weather = array[0]["main"]; //Get current time
    Serial.println(weather);
 */
    memcpy(houre1buff, &date[11], 1);
    houre1buff[1] = '\0';

    NixiClock.writeSegment(houre1buff[0] - '0', 1);

    memcpy(houre2buff, &date[12], 1);
    houre2buff[1] = '\0';

    NixiClock.writeSegment(houre2buff[0] - '0', 2);

    memcpy(minute1buff, &date[14], 1);
    minute1buff[1] = '\0';

    NixiClock.writeSegment(minute1buff[0] - '0', 3);

    memcpy(minute2buff, &date[15], 1);
    minute2buff[1] = '\0';

    NixiClock.writeSegment(minute2buff[0] - '0', 4);

    Serial.println("Houre1");
    Serial.println((uint8_t)houre1buff[0] - '0');
    Serial.println("Houre2");
    Serial.println((uint8_t)houre2buff[0] - '0');
    Serial.println("Minute1");
    Serial.println((uint8_t)minute1buff[0] - '0');
    Serial.println("Minute2");
    Serial.println((uint8_t)minute2buff[0] - '0');

    delay(2000);
  }

  else
  {
    Serial.println("Error on HTTP request");
  }

  httpWeather.begin("http://api.openweathermap.org/data/2.5/forecast?q=Freiburg,de&cnt=2&units=metric&appid=03e2fbe874af4836c6bf932b697a809b");
  int httpCodeWeather = httpWeather.GET();
  Serial.println(httpCodeWeather); //Make the request

  DeserializationError errorWeather = deserializeJson(docWeather, httpWeather.getString());

  if (errorWeather)
  {
    Serial.print(F("deserializeJson() from weather failed: "));
    Serial.println(errorWeather.c_str());
    delay(2000);
  }

  const char *forecastTime1 = docWeather["list"][0]["dt_txt"]; //Get current temp
  const int tempTime1 = docWeather["list"][0]["main"]["temp"]; //Get current time forecast

  NixiClock.writeSegment(forecastTime1[12] - '0', 2);
  NixiClock.writeSegment(9, 3);
  NixiClock.writeSegment(tempTime1, 4);

  delay(3000);

  const char *forecastTime2 = docWeather["list"][1]["dt_txt"]; //Get current time
  const int tempTime2 = docWeather["list"][1]["main"]["temp"]; //Get current time

  NixiClock.writeSegment(forecastTime2[12] - '0', 2);
  NixiClock.writeSegment(9, 3);
  NixiClock.writeSegment(tempTime2, 4);

  delay(3000);
}