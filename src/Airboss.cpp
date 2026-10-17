#include <Arduino.h>
#include "Airboss.h"
#include "Menu.h"
#include "Tuner.h"
#include "Favorites.h"


//----------------------------------------------------------
//          Constructor
Airboss::Airboss(Menu& menu1, Tuner& tuner1, Audio& audio1,
                 Timer& mainsTimer): menu(menu1), tuner(tuner1), audio(audio1), timer(mainsTimer)
{
    radioState=States::Splash;  //let's get started
    lastMenu=millis();
    lastTouch=millis();
    lastdebounce=millis();
    lastScroll=millis();
    lastbuttonpress=millis();
    lasttuner=millis();
    lastvolume=millis();
    
    //start initializing the timers save the int timer index 
    meterindex=timer.addtimer(meterInterval,true,[this](){VUmanager();}); //VU Meter
    idleindex=timer.addtimer(idleInterval,true,[this](){idleCheck();});    //sleep watchdog
    lastActivity=millis();

    


}
//-----------------------------------------------------------
Airboss::~Airboss()
{
}
//-----------------------------------------------------------
//              ​‌‌‌⁡⁣⁢⁣‍𝕊𝕋𝔸𝕋𝔼 𝕄𝔸ℂℍ𝕀ℕ𝔼 ℂ𝕆ℕ𝕋ℝ𝕆𝕃 ℂ𝔼ℕ𝕋𝔼ℝ⁡​

//                              ​‌‌‍⁡⁣⁣⁢𝗦𝗧𝗔𝗧𝗘 𝗠𝗢𝗡𝗜𝗧𝗢𝗥⁡​


void Airboss::stateMonitor(uint32_t VUleft,uint32_t VUright)
{

//save the values for VU in the class level variables
    VUL=VUleft;
    VUR=VUright;

//  ⁡⁣⁢⁣𝗠𝗲𝗻𝘂 𝗦𝗲𝗿𝘃𝗶𝗰𝗶𝗻𝗴⁡ --only service at set interval (menuInterval)
    
if((millis()-lastMenu) > menuInterval)
{
    lastMenu=millis();  //update last displayed time to now

            

    switch (radioState) //⁡⁢⁣⁣****this is the money*****/⁡
    {
    //..................................
    //          ⁡⁣⁢⁣​‌‌‍𝗦𝗽𝗹𝗮𝘀𝗵⁡​ StreamBox splash at power up
    //          (drawn in setup() so it shows while wifi connects)

    case States::Splash:
    {
        if(dirtyDisplay)
        {
            splashstart=millis();
            dirtyDisplay=false;
        }

        pendingChoice=-999;     //no menu on this screen..throw away stray taps

        if(millis()-splashstart > splashDuration)
        {
            radioState=States::Select_Genre;
            dirtyDisplay=true;
        }

        break;
    }
    //..................................
    //          ⁡⁣⁢⁣​‌‌‍𝗦𝗲𝗹𝗲𝗰𝘁_𝗚𝗲𝗻𝗿𝗲⁡​
    case States::Select_Genre:
    {
        if(genre_entry || dirtyDisplay) //only draw initial scene first time thru
        {   menu.drawFrame();
            menu.drawMenu(genreMenu);
            genre_entry=false;
            dirtyDisplay=false;
        }

        //the last line of the genre menu is "favorites"..go to that screen
        if(pendingChoice==favoritesMenuIndex)
        {
            pendingChoice=-999;
            dirtyDisplay=true;
            radioState=States::Favorites_Display;
            break;
        }

        if(pendingChoice!=-999) //a menu selection was made in touch
        {
            menu.clearMenu();

            Serial.print("Art Item: ");
            Serial.println(pendingChoice);
            menu.currGenre=pendingChoice;   //register the current genre choice


            bool gotStations=tuner.getStationChoices(tuner.genres[pendingChoice]);
            pendingChoice=-999; //reset pending choice

            dirtyDisplay=true;

            if(gotStations)
                radioState=States::Loading_Stations;
            else
                radioState=States::Server_Error;    //show message then go to favorites

            
        }



        break;
    }
    //..................................
    
    //      ⁡⁣⁢⁣​‌‌‍𝗟𝗼𝗮𝗱𝗶𝗻𝗴_𝗦𝘁𝗮𝘁𝗶𝗼𝗻𝘀 (𝗿𝗮𝗱𝗶𝗼 𝘁𝘂𝗻𝗲𝗿 𝗮𝗻𝗶𝗺𝗮𝘁𝗶𝗼𝗻⁡​

    case States::Loading_Stations:
    {
        if(dirtyDisplay)
        {
            menu.clearMenu();
            menu.drawLoadingDial();    //draw radio tuner on display
            dirtyDisplay=false;
            animationstart=millis();
            dialanimation=true; //start the animation
            Serial.print("in loading stations..");
        }

        pendingChoice=-999;     //no menu on this screen..throw away stray taps


        if(millis()-animationstart >animationDuratin)
        {
            dialanimation=false;    //end it
            radioState=States::Results_Display;
            dirtyDisplay=true;

        }





        break;
    }
    //..................................

    //      ⁡⁣⁢⁣​‌‌‍𝗥𝗲𝘀𝘂𝗹𝘁𝘀_𝗗𝗶𝘀𝗽𝗹𝗮𝘆​ 𝗮𝗳𝘁𝗲𝗿 𝘀𝗲𝗹𝗲𝗰𝘁𝗶𝗻𝗴 𝗚𝗲𝗻𝗿𝗲⁡
    
    case States::Results_Display:
    {

        if(dirtyDisplay)
        {   
            

            menu.drawFrame();
            menu.drawMenu(tuner.stationList,buildStarFlags(tuner.stationList));
            menu.draWAudioControls();

            dirtyDisplay=false;
        }




        


        menu.pausedFlag=false; //reset the pause/play to play

        //check for a selection from the list
        if(pendingChoice!=-999) //a menu selection was made in touch
        {
            
            

            selectedStation=tuner.stationList[pendingChoice];
            selectedStation.genre=menu.currGenre;   //remember the genre so a favorite keeps its art
            favorites.updateGenre(selectedStation); //already a favorite? make sure its genre is right

            Serial.println(selectedStation.Name);

            dirtyDisplay=true;
            radioState=States::Play_RadioStation;
            returnState=States::Results_Display;    //back button returns here
            pendingChoice=-999; //reset pending choice


        }


        break;
    }
    //---------------------------------------------------------
    //          ⁡⁣⁢⁣​‌‌‍𝗦𝗲𝗿𝘃𝗲𝗿_𝗘𝗿𝗿𝗼𝗿⁡​ radio-browser could not be reached
    //          show a message, then fall back to the favorites list

    case States::Server_Error:
    {
        if(dirtyDisplay)
        {
            menu.clearMenu();
            menu.drawServerError();
            errorstart=millis();
            dirtyDisplay=false;
        }

        pendingChoice=-999;     //no menu on this screen..throw away stray taps

        if(millis()-errorstart > errorDuration)
        {
            radioState=States::Favorites_Display;
            dirtyDisplay=true;
        }

        break;
    }
    //---------------------------------------------------------
    //          ⁡⁣⁢⁣​‌‌‍𝗚𝗼𝗶𝗻𝗴_𝗧𝗼_𝗦𝗹𝗲𝗲𝗽⁡​ no music for sleepTimeout
    //          warn first..a tap during the warning keeps us awake

    case States::Going_To_Sleep:
    {
        if(dirtyDisplay)
        {
            menu.clearMenu();
            menu.drawSleepScreen();
            sleepstart=millis();
            dirtyDisplay=false;
        }

        pendingChoice=-999;     //no menu on this screen..throw away stray taps

        if(lastActivity>sleepstart)     //screen was tapped after the warning went up
        {
            Serial.println("Tap..staying awake");
            radioState=States::Select_Genre;
            dirtyDisplay=true;
            break;
        }

        if(millis()-sleepstart > sleepWarning)
            goToSleep();    //does not return..wakes up as a fresh boot

        break;
    }
    //---------------------------------------------------------
    //          ⁡⁣⁢⁣​‌‌‍𝗙𝗮𝘃𝗼𝗿𝗶𝘁𝗲𝘀_𝗗𝗶𝘀𝗽𝗹𝗮𝘆⁡​ list of saved stations

    case States::Favorites_Display:
    {
        const std::vector<Stationinfo>& favList=favorites.getFavorites();

        if(dirtyDisplay)
        {
            menu.drawFrame();
            menu.drawMenu(favList,buildStarFlags(favList));  //every line gets a star
            if(favList.size()==0)
                menu.drawEmptyFavorites();
            menu.draWAudioControls();

            dirtyDisplay=false;
        }

        menu.pausedFlag=false; //reset the pause/play to play

        //check for a selection from the list
        if(pendingChoice!=-999)
        {
            if(pendingChoice<(int)favList.size())   //only real lines
            {
                selectedStation=favList[pendingChoice];

                //older favorites were saved without a genre..ask radio-browser once
                if(selectedStation.genre<0)
                {
                    selectedStation.genre=tuner.lookupGenre(selectedStation.stationuuid);
                    favorites.updateGenre(selectedStation);     //saved so we only ask once
                }

                //show the art for the genre this favorite came from
                if(selectedStation.genre>=0 && selectedStation.genre<(int)tuner.genres.size())
                    menu.currGenre=selectedStation.genre;
                else
                    menu.currGenre=-1;      //unknown..show no art rather than the wrong art

                Serial.println(selectedStation.Name);

                dirtyDisplay=true;
                radioState=States::Play_RadioStation;
                returnState=States::Favorites_Display;  //back button returns here
            }
            pendingChoice=-999; //reset pending choice
        }

        break;
    }
    //---------------------------------------------------------
    //          ⁡⁣⁢⁣​‌‌‍𝗣𝗹𝗮𝘆_𝗥𝗮𝗱𝗶𝗼𝗦𝘁𝗮𝘁𝗶𝗼𝗻 ⁡​
    
    case States::Play_RadioStation:
    {
        if(dirtyDisplay)
        {
            menu.clearMenu();
            menu.resetScroll();
            
            menu.draWAudioControls();
            menu.drawart(menu.currGenre);
            menu.drawPlayingStation(selectedStation.Name.c_str());
            menu.drawFavoriteMark(favorites.isFavorite(selectedStation));


            audio.connecttohost(selectedStation.url_resolved.c_str());
            dirtyDisplay=false;
            
        }//test-->"https://allclassical.streamguys1.com/ac96k"

        pendingChoice=-999;     //no menu on this screen..throw away stray taps


        break;
    }



    




    //..................................
    default:
        break;
    //..................................

    }
//---------⁡⁣⁢⁣end of case staatement. Past this line executed every frame cycle⁡
//---------​‌‌‍⁡⁢⁢⁣𝗘𝗡𝗗 𝗢𝗙 𝗧𝗜𝗠𝗘𝗥 𝗚𝗔𝗧𝗘𝗗 𝗖𝗢𝗡𝗧𝗥𝗢𝗟⁡​


}

//⁡⁢⁢⁢𝗼𝘂𝘁𝘀𝗶𝗱𝗲 𝗼𝗳 𝘁𝗵𝗲 𝗺𝗲𝗻𝘂 𝘁𝗶𝗺𝗶𝗻𝗴 𝗰𝗵𝗲𝗰𝗸  𝗸𝗲𝗲𝗽 𝘁𝗵𝗲⁡
//⁡⁢⁢⁢𝘀𝘁𝗮𝘁𝗶𝗼𝗻 𝘁𝗶𝘁𝗹𝗲 𝘀𝗺𝗼𝗼𝘁𝗵𝗹𝘆 𝘀𝗰𝗿𝗼𝗹𝗹𝗶𝗻𝗴⁡ ⁡⁢⁢⁢𝗮𝗻𝗱 𝘀𝗲𝗿𝗶𝘃𝗰𝗲 𝗼𝘁𝗵𝗲𝗿 𝗿𝗼𝘂𝘁𝗶𝗻𝗲𝘀⁡

if(radioState==States::Play_RadioStation && audio.isRunning())
{
       if((millis()-lastScroll)>scrollInterval)
        {
            menu.drawPlayingStation(selectedStation.Name.c_str());
            lastScroll=millis();
        }
}


if(dialanimation)   //is needle animation underway?
{
    if(millis()-lasttuner >tunerinterval)   //only on given interval
    {
        menu.animateNeedle();
        lasttuner=millis();


    }





}
//  ⁡⁣⁢⁣Volume Indicator Servicing⁡

if(volumeanimation)
{
    if(millis()-lastvolume > volumeInterval)
    {
        menu.drawVolume(tuner.currvol);
        lastvolume=millis();
    }

    //is the animation sequence over?
    if(millis()-volumestart > volumeDuratin)
    {
        Serial.printf("*************ending****");
        menu.clearVolume();
        volumeanimation=false;
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
        lastActivity=millis();  //a tap counts as activity..resets the sleep clock

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


    //   ​‌‍‌⁡⁣⁢⁣𝙏𝙤𝙤𝙡 𝙃𝙖𝙣𝙙𝙡𝙚𝙧⁡​
    if(pendingTool!=-999)   //⁡⁣⁢⁣do the tool functions⁡
    {
        menu.buttonPressed(pendingTool, radioState);  //draw button highlight
        
        
        pressedFlag=true;
        lastbuttonpress=millis();   //set to now

        switch (pendingTool)
        {
        case 0: //play / pause button
            audio.pauseResume();
            
            
            break;

        case 1:
            tuner.volumeDown();
            audio.setVolume(tuner.currvol);
            volumeanimation=true;
            volumestart=millis();
            break;
        
        case 2:
            tuner.volumeUp();
            audio.setVolume(tuner.currvol);
            volumeanimation=true;
            volumestart=millis();
            break;

        case 3: //heart button
        {
            if(radioState==States::Play_RadioStation)
            {
                //toggle: favorite -> remove it, not a favorite -> add it
                if(favorites.isFavorite(selectedStation))
                    favorites.deleteFavorites(selectedStation);
                else
                    favorites.addFavorites(selectedStation);

                menu.drawFavoriteMark(favorites.isFavorite(selectedStation));
            }
            else
                if(radioState==States::Select_Genre || radioState==States::Results_Display)
                {
                    //from a menu screen the heart is a shortcut to the favorites list
                    radioState=States::Favorites_Display;
                    dirtyDisplay=true;
                }

            break;
        }

        case 4: //back button
        {

            if(radioState==States::Play_RadioStation)
                {
                    radioState=returnState;     //results list or favorites list
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
//erase button highlight after a delay
    if(pressedFlag)
    {
        if(millis()-lastbuttonpress > buttonPressInterval)
            {
                menu.draWAudioControls();
                pressedFlag=false;
            }

        }

}  
//-----------------------------------------------------------
void Airboss::VUmanager()
{
    //  ⁡⁣⁢⁣VU Meter Servicing⁡
if(VUL>0 && radioState==States::Play_RadioStation)
{
    menu.drawMeter(VUL,VUR);
    
}
}
//----------------------------------------------------------
//  called by the timer once a second. Music playing (not paused)
//  counts as activity. After sleepTimeout with no activity, start
//  the Going_To_Sleep warning screen.
void Airboss::idleCheck()
{
    if(audio.isRunning())
        lastActivity=millis();

    //already asleep-ing or still starting up
    if(radioState==States::Going_To_Sleep || radioState==States::Splash)
        return;

    if(millis()-lastActivity > sleepTimeout)
    {
        Serial.println("No music for 10 minutes..going to sleep");
        radioState=States::Going_To_Sleep;
        dirtyDisplay=true;
    }
}
//----------------------------------------------------------
//  deep sleep: ESP32 stops almost completely. Pressing the Feather's
//  user button (GPIO 38) or reset wakes it, and it boots fresh
//  (splash screen and all)
void Airboss::goToSleep()
{
    audio.stopSong();
    menu.clearMenu();       //leave a black screen

    Serial.println("Sleeping. Press the button on GPIO 38 to wake.");
    Serial.flush();         //let the message finish sending first

    esp_sleep_enable_ext0_wakeup(GPIO_NUM_38,0);   //wake when button pulls pin LOW
    esp_deep_sleep_start();
}
//----------------------------------------------------------
void Airboss::begin()
{
    favorites.initializeFilesystem();
    favorites.loadFavorites();

    //genre menu = all the tuner genres with "favorites" added as the last line
    genreMenu=tuner.genres;
    genreMenu.push_back("favorites");
    favoritesMenuIndex=genreMenu.size()-1;

}
//----------------------------------------------------------
//  true/false for each station: is it saved as a favorite?
//  Menu uses this to decide where to draw stars
std::vector<bool> Airboss::buildStarFlags(const std::vector<Stationinfo>& stations)
{
    std::vector<bool> flags;

    for(size_t i=0; i<stations.size(); i++)
    {
        flags.push_back(favorites.isFavorite(stations[i]));
    }

    return flags;
}