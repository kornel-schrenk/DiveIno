#include "screens/LogbookScreen.h"

void LogbookScreen::init(DiveInoSettings diveInoSettings)
{
    ez.screen.clear();
    ez.header.show("Logbook");

    ez.buttons.show("Menu");
}

void LogbookScreen::handleButtonPress(String buttonName)
{
}