#pragma once

#include <Arduino.h>
#include "Menu.h"
#include "Tuner.h"
#include "Audio.h"


enum class States{Initialize,Select_Genre,Loading_Stations,Results_Display,Play_RadioStation};

class Airboss{

    private:

    //these will be instantiated in Main and passed by reference
    //then saved in this class level variable
    Menu& menu;
    Tuner& tuner;
    Audio& audio;
    

    States radioState;

    unsigned long lastMenu,lastTouch,lastdebounce,lastScroll,lastbuttonpress,
                    lasttuner;
    //service the menu 2xper second (500ms)
    //service the touch more frequently
    const unsigned long menuInterval=500;
    const unsigned long touchInterval=100;
    const unsigned long debounceInterval=150;
    const unsigned long scrollInterval=10;
    const unsigned long buttonPressInterval=200;
    const unsigned long tunerinterval=50;
    const unsigned long animationDuratin=5000;
    unsigned long animationstart;



    int pendingChoice=-999;  //a non-choice flag
    int pendingTool=-999;    //tool presses
    int needleDX=3;

    Stationinfo selectedStation;

    bool dirtyDisplay=true;
    bool menuflag=false;
    bool genre_entry=true;
    bool touchflag=false;
    bool pressedFlag=false;
    bool dialanimation=false;




    

    public:

        Airboss(Menu&, Tuner&, Audio&);
        ~Airboss();
        void stateMonitor();
        




};

