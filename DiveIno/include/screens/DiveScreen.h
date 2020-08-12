#ifndef DiveScreen_h
#define DiveScreen_h

#include "Arduino.h"
#include "M5ez.h"
#include "ezTime.h"

#include "screens/DiveInoScreen.h"

class DiveScreen : public DiveInoScreen {

    public:
        void handleButtonPress(String buttonName);

    private:
};

#endif