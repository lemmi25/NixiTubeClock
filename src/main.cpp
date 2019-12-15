#include <Arduino.h>
#include <nixiDriver.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

nixiDriver NixiClock(4, 5, 2);
StaticJsonDocument<500> doc;
HTTPClient http;

const char *ssid = "UPC3442387";
const char *password = "Ufppvydmk8mw";

void setup()
{

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  //Serial.println("Connected to the WiFi network");
}

void loop()
{

  http.begin("http://worldtimeapi.org/api/timezone/Europe/Berlin.json"); //Specify the URL
  int httpCode = http.GET();
  Serial.println("TEST"); //Make the request
  delay(5000);

  if (httpCode > 0)
  { //Check for the returning code

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, http.getString());

    if (error)
    {
      //Serial.print(F("deserializeJson() failed: "));
      //Serial.println(error.c_str());
      return;
    }

    //Get Time
    const char *date = doc["datetime"]; //Get current time

    char houre1buff[2];
    memcpy(houre1buff, &date[11], 1);
    houre1buff[1] = '\0';

    NixiClock.writeSegment(houre1buff[0] - '0', 1);

    char houre2buff[2];
    memcpy(houre2buff, &date[12], 1);
    houre2buff[1] = '\0';

    NixiClock.writeSegment(houre2buff[0] - '0', 2);

    char minute1buff[2];
    memcpy(minute1buff, &date[14], 1);
    minute1buff[1] = '\0';

    NixiClock.writeSegment(minute1buff[0] - '0', 3);

    char minute2buff[2];
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
  }

  else
  {
    Serial.println("Error on HTTP request");
  }
}