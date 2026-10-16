#include "Timer.h"


//----------------------------------------------------------------
Timer::Timer(/* args */)
{
}
//----------------------------------------------------------------
Timer::~Timer()
{
}
//----------------------------------------------------------------

int Timer::addtimer(unsigned long interval, bool activeflag, Callback cb)
{
    unsigned long now=millis();
    //use now value as lastTriggered
    timerlist.push_back({interval, now, activeflag, cb});

    return (int)timerlist.size()-1;    //the index of the new timer is its ID
}
//----------------------------------------------------------------

int Timer::timer_startstop(int ID, bool flag)
{
    if(ID<0 || ID>=(int)timerlist.size())     //error state
    {
        Serial.println("Invalid Timer ID");
        return 0;
    }

    timerlist[ID].activeFlag=flag;  //set the valid timer flag
    if(flag)
        timerlist[ID].lastTriggered=millis();    //restart the interval when started

    return 1;
}
//----------------------------------------------------------------
bool Timer::getTimerFlag(int ID)
{
    if(ID<0 || ID>=(int)timerlist.size())     //error state
    {
        Serial.println("Invalid Timer ID");
        return false;
    }

    return timerlist[ID].activeFlag;
}
//----------------------------------------------------------------

void Timer::timekeeper()
{
    unsigned long now=millis();

    for(size_t i=0; i<timerlist.size(); i++)
    {
        if(!timerlist[i].activeFlag)
            continue;

        if (now - timerlist[i].lastTriggered >= timerlist[i].interval)
        {
            timerlist[i].lastTriggered=now;     //the timer has fired..reset last
            if(timerlist[i].cb)
                timerlist[i].cb();              //trigger the callback function
        }
    }
}
