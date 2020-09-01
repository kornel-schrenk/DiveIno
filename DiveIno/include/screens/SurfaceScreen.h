#ifndef SurfaceScreen_h
#define SurfaceScreen_h

#include "Arduino.h"
#include "M5ez.h"
#include "ezTime.h"

#include "screens/DiveInoScreen.h"
#include "deco/Buhlmann.h"
#include "deco/LastDive.h"

class SurfaceScreen : public DiveInoScreen {

    public:
        void init(DiveInoSettings diveInoSettings, LastDiveData* lastDiveData);
        void handleButtonPress(String buttonName);
        void display();
    private:        
        DiveInoSettings _diveInoSettings;
        LastDive* _lastDive;
        LastDiveData* _lastDiveData = NULL;
        
        long _calculateNoFlyTime(LastDiveData* lastDiveData);
        long _calculateSurfaceInterval(LastDiveData* lastDiveData);
        void _displaySurfaceInterval(unsigned long elapsedTimeInSeconds);
};

#endif