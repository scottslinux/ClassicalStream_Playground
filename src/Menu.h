#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include "Tuner.h"
#include "digital_712pt7b.h"
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeSerif18pt7b.h>
#include "States.h"

using namespace std;

enum class ArtImage{microphone, violin, guitar, saxophone, blank};

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
    int currmenucount=0;
    int needleDX=4;
    int needleX=150;

    float leftVal=0.0;
    float rightVal=0.0;

    

    GFXcanvas16 canvas{240,45};

    

    public:
    ArtImage screenArt;
    int currGenre=0;
    int scrollX=0;
    bool pausedFlag=false;


  

        Menu(Adafruit_ILI9341& tftdisplay, XPT2046_Touchscreen& touchSense); //pass an instance of the TFT for drawing
        ~Menu();
        void drawMenu(const vector<String>&); //pass by immutable reference string vector
        void drawMenu(const vector<Stationinfo>&, const vector<bool>& starred); //overloaded..pass stationinfo structure
                                                                            //starred[i] true = draw a favorite star
        int select_from_Menu();
        void pulseCircle(bool flag);
        void draWAudioControls();
        void drawFrame();
        void drawart(ArtImage currimage);
        void drawart(int currimage);
        void drawPlayingStation(String playing);

        void clearMenu();
        void resetScroll();
        void buttonPressed(int button, States state);

        void blitImage(const uint16_t image[],int w,int h,int destx,int desty,uint16_t trans);
        void animateNeedle();
        void clearVolume();
        void drawVolume(int currvol);
        void drawMeter(uint32_t VUL, uint32_t VUR);

        void drawStar(int centerX, int centerY, int radius, uint16_t color);
        void drawFavoriteMark(bool isFavorite);    //star in the corner of the play screen
        void drawEmptyFavorites();
        void drawServerError();




};
