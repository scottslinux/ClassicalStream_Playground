#pragma once

#include <Arduino.h>
#include "Menu.h"
#include "Tuner.h"
#include "Audio.h"


enum class States{Initialize,Select_Genre,Fetch_Stations,Results_Display,Play_RadioStation};

class Airboss{

    private:

    //these will be instantiated in Main and passed by reference
    //then saved in this class level variable
    Menu& menu;
    Tuner& tuner;
    Audio& audio;
    

    States radioState;

    unsigned long lastMenu,lastTouch,lastdebounce;
    //service the menu 2xper second (500ms)
    //service the touch more frequently
    const unsigned long menuInterval=500;
    const unsigned long touchInterval=100;
    const unsigned long debounceInterval=150;

    int pendingChoice=-999;  //a non-choice flag
    int pendingTool=-999;    //tool presses
    Stationinfo selectedStation;

    bool dirtyDisplay=true;
    bool menuflag=false;
    bool genre_entry=true;
    bool touchflag=false;




    

    public:

        Airboss(Menu&, Tuner&, Audio&);
        ~Airboss();
        void stateMonitor();
        




};

