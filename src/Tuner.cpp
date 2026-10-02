#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Tuner.h"
#include <esp_random.h>
#include <algorithm>   // std::shuffle
#include <random>      // std::mt19937


//******************************************************* */
Tuner::Tuner()
{
    stationList.resize(50);
    pinMode(A4,INPUT);

    //flesh out the tag menu with Genres
    genres = {
    "rock",
    "pop",
    "classical",
    "jazz"   
    };

    



}




//******************************************************* */
Tuner::~Tuner()
{

}


//******************************************************* */
Stationinfo Tuner::getStationChoices()
{
    String payload;
    JsonDocument doc;
    

    
    HTTPClient httpclient;

    static std::mt19937 rng(esp_random());   // random number generator, seeded once


   httpclient.begin(
    "https://de1.api.radio-browser.info/json/stations/search"
    "?tag=classical"
    "&countrycode=US"
    "&limit=50"
    "&hidebroken=true"
);
    httpclient.addHeader("User-Agent", "ScottESP32Radio/1.0");  //RadioBrowser requests adding descriptive USER-AGENT

    int httpCode = httpclient.GET();

    if (httpCode > 0) 
    {
        payload = httpclient.getString();
        //Serial.println(payload);

        
    }

httpclient.end();


   
    DeserializationError jsonError = deserializeJson(doc, payload);

    if(jsonError)
      {
        Serial.print("JSON parse failed: ");
        Serial.println(jsonError.c_str());
      }
      else  //it worked!  Parse it
      {
        for(int i=0;i<50;i++)
            {
                

                stationList[i].Name = doc[i]["name"].as<String>();
                stationList[i].url_resolved= doc[i]["url_resolved"].as<String>();
                if(stationList[i].Name==NULL) break;
                
            }

            
        
      }

      for (int i=0;i<50;i++)
      {
        Serial.println("*********************************************************");
        Serial.print("Station Name: ");
        Serial.println(stationList[i].Name);
        Serial.println(stationList[i].url_resolved);
      }

      std::shuffle(stationList.begin(),stationList.end(), rng);

      for (int i=0;i<50;i++)
      {
        Serial.println("*******************SHUFFLED****************************");
        Serial.print("Station Name: ");
        Serial.println(stationList[i].Name);
        Serial.println(stationList[i].url_resolved);
      }

      int choice;
      do{
          choice=esp_random()%50; //choose a site until a valid one comes up
          

        }while (stationList[choice].Name ==NULL); 

        Serial.print("Connecting to ");
        Serial.println(stationList[choice].url_resolved);
        Serial.println(choice);

        Stationinfo currstation=stationList[choice];

        

return(currstation);
//******************************************************* */








}
        
