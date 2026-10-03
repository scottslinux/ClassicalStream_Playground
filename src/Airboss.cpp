#include <Arduino.h>
#include "Airboss.h"
#include "Menu.h"
#include "Tuner.h"


//----------------------------------------------------------
//          Constructor
Airboss::Airboss(Menu& menu1, Tuner& tuner1): menu(menu1), tuner(tuner1)
{
    radioState=States::initialize;  //let's get started
    lastMenu=millis();
    lastTouch=millis();



}
//-----------------------------------------------------------
Airboss::~Airboss()
{
}
//-----------------------------------------------------------

void Airboss::stateMonitor()
{

    if((millis()-lastMenu) > menuInterval)
        {
            menu.drawMenu(tuner.stationList); 
            lastMenu=millis();
            if(menuflag)
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

    if((millis()-lastTouch)>touchInterval)
    {
        menu.select_from_Menu();
        lastTouch=millis();
    }

}

        

