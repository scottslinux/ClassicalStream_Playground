#pragma once

#include <Arduino.h>
#include "Menu.h"
#include "Tuner.h"

enum class States{initialize,menuBrowse,selected};

class Airboss{

    private:


    

    public:

        Airboss();
        ~Airboss();
        void stateMonitor();
        




};

