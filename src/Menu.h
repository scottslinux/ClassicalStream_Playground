#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include "Tuner.h"
#include "digital_712pt7b.h"

using namespace std;


class Menu{
    private:

    Adafruit_ILI9341& Display;
    XPT2046_Touchscreen& touchScreen;

    //define the bounds/parameters of the menu
    const int menuWidth=200; 
    const int menuHeight=280;
    const int maxchars=19;
    int menuoffX=0;
    int menuoffY=0;
    int cellheight=0;
    int choices=0;
    int cols=0;




    public:

        Menu(Adafruit_ILI9341& tftdisplay, XPT2046_Touchscreen& touchSense); //pass an instance of the TFT for drawing
        ~Menu();
        void drawMenu(const vector<String>&); //pass by immutable reference string vector
        void drawMenu(const vector<Stationinfo>&); //overloaded..pass stationinfo structure
        int select_from_Menu();
        void pulseCircle(bool flag);
        



};
