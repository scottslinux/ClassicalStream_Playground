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
            int genre=-1;       //index into genres (picks the art). -1 = unknown

        };


class Tuner
{
    private:

        

        




    public:

        int currvol=13;     //starting volume 0..21
        int volDx=1;
        std::vector<String> genres;
        std::vector<Stationinfo> stationList;   //make this list available to main

        Tuner();
        ~Tuner();

        bool getStationChoices(String genreChoice);    //false = server unreachable
        int lookupGenre(String uuid);                  //-1 = unknown
        void volumeUp();
        void volumeDown();
        




};
