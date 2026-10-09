#include <Arduino.h>
#include "Airboss.h"
#include "Menu.h"
#include "Tuner.h"


//----------------------------------------------------------
//          Constructor
Airboss::Airboss(Menu& menu1, Tuner& tuner1, Audio& audio1): menu(menu1), tuner(tuner1), audio(audio1)
{
    radioState=States::Select_Genre;  //let's get started
    lastMenu=millis();
    lastTouch=millis();
    lastdebounce=millis();




}
//-----------------------------------------------------------
Airboss::~Airboss()
{
}
//-----------------------------------------------------------
//              ​‌‌‌⁡⁣⁢⁣‍𝕊𝕋𝔸𝕋𝔼 𝕄𝔸ℂℍ𝕀ℕ𝔼 ℂ𝕆ℕ𝕋ℝ𝕆𝕃 ℂ𝔼ℕ𝕋𝔼ℝ⁡​

//                              ​‌‌‍⁡⁣⁣⁢𝗦𝗧𝗔𝗧𝗘 𝗠𝗢𝗡𝗜𝗧𝗢𝗥⁡​


void Airboss::stateMonitor()
{

//  ⁡⁣⁢⁣𝗠𝗲𝗻𝘂 𝗦𝗲𝗿𝘃𝗶𝗰𝗶𝗻𝗴⁡ --only service at set interval (menuInterval)
    
if((millis()-lastMenu) > menuInterval)
{
    lastMenu=millis();  //update last displayed time to now

            

    switch (radioState) //⁡⁢⁣⁣****this is the money*****/⁡
    {
    //..................................
    //          ⁡⁣⁢⁣𝗦𝗲𝗹𝗲𝗰𝘁_𝗚𝗲𝗻𝗿𝗲⁡
    case States::Select_Genre:
    {
        if(genre_entry || dirtyDisplay) //only draw initial scene first time thru
        {   menu.drawFrame();
            menu.drawMenu(tuner.genres);
            genre_entry=false;
            dirtyDisplay=false;
        }

        
        if(pendingChoice!=-999) //a menu selection was made in touch
        {
            menu.clearMenu();

            Serial.print("Art Item: ");
            Serial.println(pendingChoice);

            menu.drawart(pendingChoice);

            tuner.getStationChoices(tuner.genres[pendingChoice]);
            pendingChoice=-999; //reset pending choice

            dirtyDisplay=true;  
            
            radioState=States::Results_Display;
            
            
        }



        break;
    }
    //..................................
    //      ⁡⁣⁢⁣𝗥𝗲𝘀𝘂𝗹𝘁𝘀_𝗗𝗶𝘀𝗽𝗹𝗮𝘆 𝗮𝗳𝘁𝗲𝗿 𝘀𝗲𝗹𝗲𝗰𝘁𝗶𝗻𝗴 𝗚𝗲𝗻𝗿𝗲⁡
    
    case States::Results_Display:
    {

        if(dirtyDisplay)
        {   
            

            menu.drawFrame();
            menu.drawart(ArtImage::blank);
            menu.drawMenu(tuner.stationList);
            menu.draWAudioControls();

            dirtyDisplay=false;
        }

        //check for a selection from the list
        if(pendingChoice!=-999) //a menu selection was made in touch
        {
            
            

            selectedStation=tuner.stationList[pendingChoice];

            Serial.println(selectedStation.Name);

            dirtyDisplay=true;  
            radioState=States::Play_RadioStation;
            pendingChoice=-999; //reset pending choice
            
            
        }


        break;
    }
    //---------------------------------------------------------
    //          ⁡⁣⁢⁣𝗣𝗹𝗮𝘆_𝗥𝗮𝗱𝗶𝗼𝗦𝘁𝗮𝘁𝗶𝗼𝗻 ⁡
    
    case States::Play_RadioStation:
    {
        if(dirtyDisplay)
        {
            menu.clearMenu();
            
            menu.draWAudioControls();
            menu.drawart(menu.currGenre);
            menu.drawPlayingStation(selectedStation.Name.c_str());
            

            audio.connecttohost(selectedStation.url_resolved.c_str());
            dirtyDisplay=false;
            
        }//test-->"https://allclassical.streamguys1.com/ac96k"


        break;
    }



    




    //..................................
    default:
        break;
    //..................................

    }


            
            
    if(menuflag)    //pulse circle effect
    {
        menu.pulseCircle(menuflag);
        menuflag=false;
    }
        else
        {
            menu.pulseCircle(menuflag);
            menuflag=true;
        }
    

}


//      ⁡⁣⁢⁣𝗧𝗼𝘂𝗰𝗵 𝗽𝗼𝗹𝗹𝗶𝗻𝗴 𝗼𝗻 𝘁𝗼𝘂𝗰𝗵𝗜𝗻𝘁𝗲𝗿𝘃𝗮𝗹⁡
if((millis()-lastTouch)>touchInterval)
{
    if ((millis()-lastdebounce)>debounceInterval)
    {
    int choice=menu.select_from_Menu();
    if(choice!=-999)//  only save real taps
    {
        if(choice<=10)
            pendingChoice=choice;

        if(choice>=100) //tool choice
        {   pendingTool=choice-100;
            Serial.print("tool: ");
            Serial.println(pendingTool);

        }
    }
        lastdebounce=millis();  //reset debounce clock
    }
    lastTouch=millis();



    if(pendingTool!=-999)   //⁡⁣⁢⁣do the tool functions⁡
    {
        switch (pendingTool)
        {
        case 0: //play / pause button
            audio.pauseResume();
            break;

        case 1:
            tuner.volumeDown();
            audio.setVolume(tuner.currvol);
            break;
        
        case 2:
            tuner.volumeUp();
            audio.setVolume(tuner.currvol);
            break;

        case 4: //back button
        {
            
            if(radioState==States::Play_RadioStation)
                {
                    radioState=States::Results_Display;
                    audio.stopSong();
                    dirtyDisplay=true;
                }
                else
                    if(radioState!=States::Select_Genre)
                    {
                        radioState=States::Select_Genre;
                        dirtyDisplay=true;
                    }

            
            break;
        }
        
        default:
            break;
        }







        
        pendingTool=-999;   //reset tool command

    }


    

}

}  

