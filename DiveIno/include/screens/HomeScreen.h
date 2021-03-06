#ifndef HomeScreen_h
#define HomeScreen_h

#include "Arduino.h"
#include "M5ez.h"
#include "ezTime.h"

#include "screens/DiveInoScreen.h"
#include "utils/TimeUtils.h"
#include "deco/LastDive.h"

class HomeScreen : public DiveInoScreen {

    public:
        HomeScreen(TimeUtils timeUtils, LastDive* lastDive);
        void initHomeScreen(DiveInoSettings diveInoSettings);
        
        void displayHomeClock();
        void updateTime();
        void updateDate();
        void updateAmPm();

        int handleButtonPress(String buttonName);

    private:
        LastDive* _lastDive;
        bool _isNoFlyDisplayed();

        bool _isMinimalModeActive = false;
        DiveInoSettings _diveInoSettings;
        TimeUtils _timeUtils;
};

#endif