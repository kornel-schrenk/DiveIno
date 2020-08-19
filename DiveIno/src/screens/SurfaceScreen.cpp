#include "screens/SurfaceScreen.h"

void SurfaceScreen::init(DiveInoSettings diveInoSettings)
{
    ez.screen.clear();
    ez.header.show("Surface");

    ez.buttons.show("# # Menu");
}

void SurfaceScreen::handleButtonPress(String buttonName)
{
}