#ifndef SurfaceScreen_h
#define SurfaceScreen_h

#include "Arduino.h"
#include "M5ez.h"
#include "ezTime.h"

#include "screens/DiveInoScreen.h"

class SurfaceScreen : public DiveInoScreen {

    public:
        void init(DiveInoSettings diveInoSettings);
        
        void handleButtonPress(String buttonName);

    private:
};

#endif