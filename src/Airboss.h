#pragma once

#include <Arduino.h>
#include "Menu.h"
#include "Tuner.h"



enum class States{initialize,menuBrowse,selected};

class Airboss{

    private:

    //these will be instantiated in Main and passed by reference
    //then saved in this class level variable
    Menu& menu;
    Tuner& tuner;

    States radioState;
    unsigned long lastMenu,lastTouch;
    //service the menu 2xper second (500ms)
    //service the touch more frequently
    const unsigned long menuInterval=500;
    const unsigned long touchInterval=200;
    bool dirtyDisplay=false;
    bool menuflag=false;


    

    public:

        Airboss(Menu&, Tuner&);
        ~Airboss();
        void stateMonitor();
        




};

