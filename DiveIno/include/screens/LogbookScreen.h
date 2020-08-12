#ifndef LogbookScreen_h
#define LogbookScreen_h

#include "Arduino.h"
#include "M5ez.h"
#include "ezTime.h"

#include "screens/DiveInoScreen.h"

class LogbookScreen : public DiveInoScreen {

    public:
        void handleButtonPress(String buttonName);

    private:
};

#endif