#pragma once

#include <Arduino.h>
#include "Menu.h"
#include "Tuner.h"
#include "Audio.h"
#include "States.h"
#include "Timer.h"
#include "Favorites.h"



class Airboss{

    private:

    //these will be instantiated in Main and passed by reference
    //then saved in this class level variable
    Menu& menu;
    Tuner& tuner;
    Audio& audio;
    Timer& timer;

    States radioState;
    Favorites favorites;
    

    unsigned long lastMenu,lastTouch,lastdebounce,lastScroll,lastbuttonpress,
                    lasttuner,lastvolume;
    //service the menu 2xper second (500ms)
    //service the touch more frequently
    const unsigned long menuInterval=500;
    const unsigned long touchInterval=100;
    const unsigned long debounceInterval=150;
    const unsigned long scrollInterval=10;
    const unsigned long buttonPressInterval=200;
    const unsigned long tunerinterval=50;
    const unsigned long animationDuratin=5000;
    const unsigned long volumeDuratin=2000;
    const unsigned long volumeInterval=50;
    const unsigned long meterInterval=50;
    const unsigned long errorDuration=4000;    //how long the server error shows
    const unsigned long splashDuration=2000;   //extra splash time once loop() starts
    const unsigned long sleepTimeout=600000;   //10 minutes with no music -> sleep
    const unsigned long sleepWarning=10000;    //"going to sleep" shows this long
    const unsigned long idleInterval=1000;     //check for idle once a second

    int meterindex=0;
    int idleindex=0;

    unsigned long animationstart;
    unsigned long volumestart;
    unsigned long errorstart;
    unsigned long splashstart;
    unsigned long sleepstart;
    unsigned long lastActivity;     //last time music was playing or the screen was touched



    int pendingChoice=-999;  //a non-choice flag
    int pendingTool=-999;    //tool presses
    uint32_t VUL=0;
    uint32_t VUR=0;

    Stationinfo selectedStation;

    std::vector<String> genreMenu;      //tuner genres + "favorites" at the end
    int favoritesMenuIndex=0;           //which line of genreMenu is "favorites"
    States returnState=States::Results_Display;   //where back goes from Play_RadioStation

    bool dirtyDisplay=true;
    bool genre_entry=true;
    bool pressedFlag=false;
    bool dialanimation=false;
    bool volumeanimation=false;

    std::vector<bool> buildStarFlags(const std::vector<Stationinfo>& stations);





    

    public:

        Airboss(Menu&, Tuner&, Audio&, Timer& mainsTimer);
        ~Airboss();
        void stateMonitor(uint32_t VUleft,uint32_t VUright);
        void VUmanager();
        void idleCheck();
        void goToSleep();
        void begin();
        




};

