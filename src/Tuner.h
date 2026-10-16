#pragma once
#include <Arduino.h>
#include <HTTPClient.h>
#include <random>
#include <algorithm>

//keep this struct outside of class...can be used by main without scope modifier
struct Stationinfo{
            String Name;
            String url_resolved;
            String stationuuid;
      
        };


class Tuner
{
    private:

        

        




    public:

        int currvol=15;
        int volDx=1;
        std::vector<String> genres;
        std::vector<Stationinfo> stationList;   //make this list available to main

        Tuner();
        ~Tuner();

        bool getStationChoices(String genreChoice);    //false = server unreachable
        void volumeUp();
        void volumeDown();
        




};
