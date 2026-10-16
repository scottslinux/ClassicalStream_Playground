#include "Favorites.h"

//where the favorites list lives on the LittleFS partition
const char* favoritesPath="/favorites/favorites.json";

Favorites::Favorites()
{

}
//--------------------------------------------------------------
Favorites::~Favorites()
{
}
//--------------------------------------------------------------

//  two stations are the same if their radio-browser uuid matches
//  (fall back to the stream url if a uuid is missing)
bool Favorites::sameStation(const Stationinfo& a, const Stationinfo& b)
{
    if(a.stationuuid.length()>0 && b.stationuuid.length()>0)
        return a.stationuuid==b.stationuuid;

    return a.url_resolved==b.url_resolved;
}
//--------------------------------------------------------------
bool Favorites::isFavorite(const Stationinfo& station)
{
    for(size_t i=0; i<favoritesList.size(); i++)
    {
        if(sameStation(favoritesList[i],station))
            return true;
    }
    return false;
}
//--------------------------------------------------------------
//  add a station to the list and save the list to LittleFS
bool Favorites::addFavorites(const Stationinfo& station)
{
    if(isFavorite(station))
    {
        Serial.println("Already a favorite");
        return false;
    }

    if(favoritesCount>=maxFavorites)
    {
        Serial.println("Favorites list is full");
        return false;
    }

    favoritesList.push_back(station);
    favoritesCount=favoritesList.size();
    saveFavorites();

    Serial.print("Added favorite: ");
    Serial.println(station.Name);
    return true;
}
//--------------------------------------------------------------
//  remove a station from the list and save the list to LittleFS
void Favorites::deleteFavorites(const Stationinfo& station)
{
    for(size_t i=0; i<favoritesList.size(); i++)
    {
        if(sameStation(favoritesList[i],station))
        {
            favoritesList.erase(favoritesList.begin()+i);
            favoritesCount=favoritesList.size();
            saveFavorites();

            Serial.print("Removed favorite: ");
            Serial.println(station.Name);
            return;
        }
    }
}
//--------------------------------------------------------------
const std::vector<Stationinfo>& Favorites::getFavorites()
{
    return favoritesList;
}
//--------------------------------------------------------------
void Favorites::initializeFilesystem()
{
    if(!LittleFS.begin(true))
    {
        Serial.println("Mount Failed during initialization!");
        return;     // no filesystem, so don't try to make a folder
    }

    Serial.println("LittleFS Successfully Mounted");

    // make the favorites folder the first time only
    if(!LittleFS.exists("/favorites"))
    {
        if(LittleFS.mkdir("/favorites"))
        {
            Serial.println("Created /favorites");
        }
        else
        {
            Serial.println("Failed to create /favorites");
        }

        dumpFilesystem();
    }
}
//-------------------------------------------------------------
void Favorites::dumpFilesystem()
{
    File outFile=LittleFS.open("/favorites.txt","r");
    if(outFile){
        String text= outFile.readString();
        Serial.print(text);
        outFile.close();
    }
}

//-------------------------------------------------------------
//  write every station in favoritesList out to the json file
//  file looks like:
//  [
//    {"name":"...","url_resolved":"...","stationuuid":"..."},
//    {"name":"...","url_resolved":"...","stationuuid":"..."}
//  ]
void Favorites::saveFavorites()
{
    JsonDocument doc;
    JsonArray stations=doc.to<JsonArray>();     //the top level of the file is an array

    for(size_t i=0; i<favoritesList.size(); i++)
    {
        JsonObject station=stations.add<JsonObject>();  //add an empty {} to the array

        station["name"]=favoritesList[i].Name;
        station["url_resolved"]=favoritesList[i].url_resolved;
        station["stationuuid"]=favoritesList[i].stationuuid;
    }

    File outFile=LittleFS.open(favoritesPath,"w");  //"w" replaces the old file
    if(!outFile)
    {
        Serial.println("Could not open favorites file for writing");
        return;
    }

    serializeJson(doc,outFile);     //json text goes straight into the file
    outFile.close();

    Serial.print("Saved favorites: ");
    Serial.println(favoritesList.size());
}
//-------------------------------------------------------------
//  read the json file and rebuild favoritesList from it
void Favorites::loadFavorites()
{
    favoritesList.clear();      //start from an empty list
    favoritesCount=0;

    if(!LittleFS.exists(favoritesPath))     //first boot..nothing saved yet
    {
        Serial.println("No favorites file yet");
        return;
    }

    File inFile=LittleFS.open(favoritesPath,"r");
    if(!inFile)
    {
        Serial.println("Could not open favorites file for reading");
        return;
    }

    JsonDocument doc;
    DeserializationError error=deserializeJson(doc,inFile);  //read json text from the file
    inFile.close();

    if(error)
    {
        Serial.print("Favorites file is bad: ");
        Serial.println(error.c_str());
        return;
    }

    JsonArray stations=doc.as<JsonArray>();

    for(size_t i=0; i<stations.size(); i++)
    {
        JsonObject station=stations[i];

        Stationinfo fav;
        fav.Name=station["name"].as<String>();
        fav.url_resolved=station["url_resolved"].as<String>();
        fav.stationuuid=station["stationuuid"].as<String>();

        favoritesList.push_back(fav);
    }

    favoritesCount=favoritesList.size();

    Serial.print("Loaded favorites: ");
    Serial.println(favoritesCount);
}
//-------------------------------------------------------------

