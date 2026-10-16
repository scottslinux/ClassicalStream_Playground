#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "Tuner.h"
#include <esp_random.h>
#include <algorithm>   // std::shuffle
#include <random>      // std::mt19937

#define stationNum 20
//******************************************************* */
Tuner::Tuner()
{
    stationList.resize(stationNum);
    pinMode(A4,INPUT);

    //flesh out the tag vector with Genres
    genres = {
    "rock",
    "pop",
    "classical",
    "jazz",
    "smooth jazz",
    "oldies",
    "news",
    "talk"   
    };

    



}




//******************************************************* */
Tuner::~Tuner()
{

}


//******************************************************* */
//  returns true if a station list was loaded, false if the radio-browser
//  server could not be reached (or sent back something unusable)
bool Tuner::getStationChoices(String genreChoice)
{
    String payload;
    JsonDocument doc;



    HTTPClient httpclient;

    static std::mt19937 rng(esp_random());   // random number generator, seeded once

    //no wifi..don't even try
    if(WiFi.status()!=WL_CONNECTED)
    {
        Serial.println("No WiFi..cannot reach radio-browser");
        return false;
    }

    String test=genreChoice;
    unsigned long now=millis();

   httpclient.begin(
    "http://de2.api.radio-browser.info/json/stations/search"
    "?tag="+test+
    "&countrycode=US"
    "&limit=20"
    "&hidebroken=true"
);
    httpclient.setConnectTimeout(4000);    //give up connecting after 4 seconds
    httpclient.setTimeout(5000);           //give up waiting for data after 5 seconds

    httpclient.addHeader("User-Agent", "ScottESP32Radio/1.0");  //RadioBrowser requests adding descriptive USER-AGENT

    Serial.println("Starting GET");

    int httpCode = httpclient.GET();

    //negative code = could not connect at all, other codes = server trouble
    if (httpCode != HTTP_CODE_OK)
    {
        Serial.print("Radio-browser request failed. Code: ");
        Serial.print(httpCode);
        Serial.print(" ");
        Serial.println(httpclient.errorToString(httpCode));
        httpclient.end();
        return false;
    }

    payload = httpclient.getString();
    Serial.print("HTTP Code: ");
    Serial.println(httpCode);
    Serial.println(millis()-now);

httpclient.end();



    DeserializationError jsonError = deserializeJson(doc, payload);

    if(jsonError)
      {
        Serial.print("JSON parse failed: ");
        Serial.println(jsonError.c_str());
        return false;
      }
      else  //it worked!  Parse it
      {
        for(int i=0;i<stationNum;i++)
            {
                

                stationList[i].Name = doc[i]["name"].as<String>();
                stationList[i].url_resolved= doc[i]["url_resolved"].as<String>();
                stationList[i].stationuuid=doc[i]["stationuuid"].as<String>();
                if(stationList[i].Name==NULL) break;
                
            }

            
        
      }

      std::shuffle(stationList.begin(),stationList.end(), rng);

return true;
}
//******************************************************* */
void Tuner::volumeUp()
{
  if((currvol+volDx)<=21)
    currvol+=volDx;

    
  
}
//******************************************************* */
void Tuner::volumeDown()
{
  if((currvol-volDx)>=0)
    currvol-=volDx;

    
  
}
//******************************************************* */








        
