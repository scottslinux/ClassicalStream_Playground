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
    test.replace(" ","%20");    //spaces are not allowed in a URL ("smooth jazz" -> "smooth%20jazz")
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
//  ask radio-browser for one station's tags and match them to our genres.
//  tags come back like "monterey,smooth jazz"
//  returns the genre index, or -1 if it can't tell (or can't reach the server)
int Tuner::lookupGenre(String uuid)
{
    if(uuid.length()==0 || WiFi.status()!=WL_CONNECTED)
        return -1;

    HTTPClient httpclient;
    JsonDocument doc;

    httpclient.begin("http://de2.api.radio-browser.info/json/stations/byuuid/"+uuid);
    httpclient.setConnectTimeout(4000);
    httpclient.setTimeout(5000);
    httpclient.addHeader("User-Agent", "ScottESP32Radio/1.0");

    int httpCode=httpclient.GET();
    if(httpCode!=HTTP_CODE_OK)
    {
        Serial.print("Genre lookup failed. Code: ");
        Serial.println(httpCode);
        httpclient.end();
        return -1;
    }

    String payload=httpclient.getString();
    httpclient.end();

    if(deserializeJson(doc,payload))   //true means there was an error
        return -1;

    String tags=doc[0]["tags"].as<String>();
    tags.toLowerCase();
    tags.replace(", ",",");             //"jazz, smooth jazz" -> "jazz,smooth jazz"
    tags=","+tags+",";                  //commas on both ends so every tag is ",tag,"

    Serial.print("Station tags: ");
    Serial.println(tags);

    //the longest matching genre wins, so "smooth jazz" beats "jazz"
    int best=-1;
    int bestLength=0;
    for(size_t i=0; i<genres.size(); i++)
    {
        String wanted=","+genres[i]+",";
        if(tags.indexOf(wanted)>=0 && (int)genres[i].length()>bestLength)
        {
            best=i;
            bestLength=genres[i].length();
        }
    }

    return best;
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








        
