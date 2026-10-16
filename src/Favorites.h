#pragma once

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <vector>
#include "Tuner.h"



class Favorites
{
private:

    std::vector<Stationinfo> favoritesList;
    int favoritesCount=0;
    const int maxFavorites=10;      //the menu can only show 10 lines

    bool sameStation(const Stationinfo& a, const Stationinfo& b);


public:
    Favorites();
    ~Favorites();

    void initializeFilesystem();
    void loadFavorites();
    void saveFavorites();
    bool addFavorites(const Stationinfo& station);     //false if full or already there
    void deleteFavorites(const Stationinfo& station);
    bool isFavorite(const Stationinfo& station);
    const std::vector<Stationinfo>& getFavorites();  //read only handle to the list
    void dumpFilesystem();
};




