#pragma once

#include <functional>
#include <vector>
#include <Arduino.h>


using Callback= std::function<void()>;

class Timer
{

private:

struct Timer_struct
{
    unsigned long interval;
    unsigned long lastTriggered;
    bool activeFlag;
    Callback cb;
};

std::vector<Timer_struct> timerlist; //this will hold all of the registered timers





public:
    Timer(/* args */);
    ~Timer();

    int addtimer(unsigned long interval,bool flag, Callback cb);
    void timekeeper();
    int timer_startstop(int index, bool flag);
    bool getTimerFlag(int ID);







};

