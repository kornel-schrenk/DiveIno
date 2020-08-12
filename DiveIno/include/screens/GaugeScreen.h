#ifndef GaugeScreen_h
#define GaugeScreen_h

#include "Arduino.h"
#include "M5ez.h"
#include "ezTime.h"

#include "screens/DiveInoScreen.h"

class GaugeScreen : public DiveInoScreen {

    public:
        void handleButtonPress(String buttonName);

    private:
};

#endif