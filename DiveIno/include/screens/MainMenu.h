#ifndef MainMenu_h
#define MainMenu_h

#include "Arduino.h"
#include "M5ez.h"
#include "ezTime.h"

#include "images/jpgs.h"
#include "images/jpgsdark.h"

#include "screens/DiveInoScreen.h"

class MainMenu : public DiveInoScreen {

    public:
        ezMenu initMainMenu();
    private:

};

#endif