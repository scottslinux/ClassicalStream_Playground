#include <Arduino.h>
#include <HTTPClient.h>
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
    "jazz"   
    };

    



}




//******************************************************* */
Tuner::~Tuner()
{

}


//******************************************************* */
Stationinfo Tuner::getStationChoices(String genreChoice)
{
    String payload;
    JsonDocument doc;
    

    
    HTTPClient httpclient;

    static std::mt19937 rng(esp_random());   // random number generator, seeded once

    String test=genreChoice;

   httpclient.begin(
    "https://de1.api.radio-browser.info/json/stations/search"
    "?tag="+test+
    "&countrycode=US"
    "&limit=20"
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
        for(int i=0;i<stationNum;i++)
            {
                

                stationList[i].Name = doc[i]["name"].as<String>();
                stationList[i].url_resolved= doc[i]["url_resolved"].as<String>();
                if(stationList[i].Name==NULL) break;
                
            }

            
        
      }
/*
      for (int i=0;i<stationNum;i++)
      {
        Serial.println("*********************************************************");
        Serial.print("Station Name: ");
        Serial.println(stationList[i].Name);
        Serial.println(stationList[i].url_resolved);
      }

      

      for (int i=0;i<stationNum;i++)
      {
        Serial.println("*******************SHUFFLED****************************");
        Serial.print("Station Name: ");
        Serial.println(stationList[i].Name);
        Serial.println(stationList[i].url_resolved);
      }
*/
      std::shuffle(stationList.begin(),stationList.end(), rng);
      
      int choice;
      do{
          choice=esp_random()%stationNum; //choose a site until a valid one comes up
          

        }while (stationList[choice].Name ==NULL); 

        Serial.print("Connecting to ");
        Serial.println(stationList[choice].url_resolved);
        Serial.println(choice);

        Stationinfo currstation=stationList[choice];

        

return(currstation);
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








        
