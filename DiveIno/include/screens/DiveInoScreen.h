#ifndef DiveInoScreen_h
#define DiveInoScreen_h

#define SCREEN_HOME       310
#define SCREEN_DIVE       320
#define SCREEN_GAUGE      330
#define SCREEN_LOGBOOK    340
#define SCREEN_SURFACE    350
#define SCREEN_SETTINGS   360
#define SCREEN_ABOUT      370

#include "images/jpgs.h"
#include "images/jpgsdark.h"

#include "DiveInoBase.h"

class DiveInoScreen : public DiveInoBase {

    public: 
        void refreshClockWidget();
        bool isClockWidgetDisplayed();
    
    private:
};

#endif