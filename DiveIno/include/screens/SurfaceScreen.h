#ifndef SurfaceScreen_h
#define SurfaceScreen_h

#include "Arduino.h"
#include "M5ez.h"
#include "ezTime.h"

#include "screens/DiveInoScreen.h"
#include "deco/Buhlmann.h"

class SurfaceScreen : public DiveInoScreen {

    public:
        void init(DiveInoSettings diveInoSettings, DiveResult* diveResult);
        void handleButtonPress(String buttonName);
    private:
         DiveResult* _diveResult;
         DiveInoSettings _diveInoSettings;
         void _display();
};

#endif