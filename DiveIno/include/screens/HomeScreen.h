#ifndef HomeScreen_h
#define HomeScreen_h

#include "Arduino.h"
#include "M5ez.h"
#include "ezTime.h"

#include "screens/DiveInoScreen.h"

class HomeScreen : public DiveInoScreen {

    public:
        void initHomeScreen(DiveInoSettings diveInoSettings);
        
        void displayHomeClock();
        void updateTime();
        void updateDate();
        void updateAmPm();

        void handleButtonPress(String buttonName);
        
        String getTimezoneLocation();
        void storeTimeInRtc();

    private:
        bool _isMinimalModeActive = false;
        DiveInoSettings _diveInoSettings;
};

#endif