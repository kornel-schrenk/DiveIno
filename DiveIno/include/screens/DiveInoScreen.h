#ifndef DiveInoScreen_h
#define DiveInoScreen_h

#define SCREEN_HOME       310
#define SCREEN_DIVE       320
#define SCREEN_GAUGE      330
#define SCREEN_LOGBOOK    340
#define SCREEN_SURFACE    350
#define SCREEN_SETTINGS   360
#define SCREEN_ABOUT      370

class DiveInoScreen {

    public: 
        void refreshClockWidget();
        bool isClockWidgetDisplayed();
    
    private:
};

#endif