#pragma once
#include <Arduino.h>
#include <HTTPClient.h>
#include <random>
#include <algorithm>

//keep this struct outside of class...can be used by main without scope modifier
struct Stationinfo{
            String Name;
            String url_resolved;
      
        };


class Tuner
{
    private:

        std::vector<String> genres;
        




    public:

        std::vector<Stationinfo> stationList;   //make this list available to main

        Tuner();
        ~Tuner();

        Stationinfo getStationChoices();
        




};
