#include <Arduino.h>
#include <Preferences.h>
#include <SPI.h>
#include <WiFi.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include <XPT2046_Touchscreen.h>
#include "Audio.h" //see my repository at github "https://github.com/schreibfaul1/ESP32-audioI2S"
#include "secrets.h"
#include "Tuner.h"
#include "Menu.h"
#include "Airboss.h"





#define I2S_BCLK  33     // MAX98357A BCLK
#define I2S_LRC   27     // MAX98357A LRC / WS
#define I2S_DOUT  15     // MAX98357A DIN

#define CS 14
#define DC 12
#define Reset 32
#define touch_CS 13

uint8_t max_volume   = 21;
uint8_t cur_volume =15;

Adafruit_ILI9341 tft(CS,DC,Reset);
XPT2046_Touchscreen ts(touch_CS);

// instantiate the objects
Audio audio;
Tuner tuner;
Menu menu(tft, ts); //send a handle to the tft & touch context to menu
Airboss airboss(menu,tuner);

double lastmenu=millis();   //start the clock
float menuInterval=500.0;
bool menuflag=false;

double lastTouch=millis();
float touchInterval=200.0;



#define Push_Button A5


//------------------------------------------------------------------------------
void connectWiFi()  //connect to wifi.
{
    WiFi.begin(SSID,PASSWORD);
    Serial.print("Connecting");
    int tries=0;

    while(WiFi.status()!=WL_CONNECTED && tries<10)
    {
        Serial.print(".");
        delay(1000);
        tries++;

    }
    if(WiFi.status()==WL_CONNECTED)
    {
        Serial.println();
        Serial.println("Connected !!!");
        Serial.println(WiFi.localIP());
        delay(3000);
    }
        else
            Serial.println("Unable to Connect to WiFi");



}


//------------------------------------------------------------------------------
void setup() {

    Serial.begin(115200);
    delay(1000);

    tft.begin();
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextWrap(false); //disable wrapping tot he tft


    connectWiFi();      //go get some

    Stationinfo newStation=tuner.getStationChoices();

    ts.begin(); //touchscreen begin

    




    cur_volume=max_volume* 0.65;

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(cur_volume); // 0...21
    audio.connecttohost(newStation.url_resolved.c_str());

    Serial.println("---------------");
    Serial.println((newStation.Name).c_str());


    
    tft.fillRoundRect(0,0,240,320,10,ILI9341_WHITE);
    tft.fillRoundRect(6,6,228,308,10,ILI9341_BLACK);
    
    



    tft.setFont(&digital_712pt7b);
    tft.setTextColor(ILI9341_CYAN);
    
    
/*
    for(int titles=0;titles<10;titles++)
    {
        tft.setCursor(10,50+(25*titles));
        tft.println(tuner.stationList[titles].Name.c_str());
        

    }

    //pinMode(Push_Button, INPUT_PULLUP);
*/
    vector<String> test={"some station","Another one","A longer station than oyu can imagine",
        "one more..."};
    menu.drawMenu(tuner.stationList);    //test the handle!
}

//-----------------------------------------------------
void loop() 
{

    
    audio.loop();    // must hit this every time

    airboss.stateMonitor(); // manage program floaw
    

}