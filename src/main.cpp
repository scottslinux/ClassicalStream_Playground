#include <Arduino.h>
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
#include "Timer.h"





#define I2S_BCLK  33     // MAX98357A BCLK
#define I2S_LRC   27     // MAX98357A LRC / WS
#define I2S_DOUT  15     // MAX98357A DIN

#define CS 14
#define DC 12
#define Reset 32
#define touch_CS 13

uint32_t vuLeft = 0;    //VU Meter values
uint32_t vuRight = 0;

Adafruit_ILI9341 tft(CS,DC,Reset);
XPT2046_Touchscreen ts(touch_CS);

// instantiate the objects
Timer timer;    //master timer...will pass to airboss
Audio audio;
Tuner tuner;
Menu menu(tft, ts); //send a handle to the tft & touch context to menu
Airboss airboss(menu,tuner,audio,timer); //airboss needs control of assets 
                                        //everything but display

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
//--------------------------------------------------------------

void my_audio_info(Audio::msg_t m) {
    if (m.e == Audio::evt_vu && m.vec1.size() >= 2) {
        vuLeft  = m.vec1[0];
        vuRight = m.vec1[1];
    }
}
//------------------------------------------------------------------------------
void setup() {

    Serial.begin(115200);
    delay(1000);

    airboss.begin();

    tft.begin();
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextWrap(false); //disable wrapping tot he tft

    menu.drawSplash();      //StreamBox splash stays up while wifi connects


    connectWiFi();      //go get some

    ts.begin(); //touchscreen begin

    //register the meter call back function
    Audio::audio_info_callback = my_audio_info;


    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(tuner.currvol); // 0...21  tuner keeps track of the volume
    audio.settings.VU_LEVEL = true; //set up the meter
}

//-----------------------------------------------------
void loop() 
{
    audio.loop();    // must hit this every time

    airboss.stateMonitor(vuLeft,vuRight); // manage program flow
    timer.timekeeper();                   // update the timers every frame

}