#include "screens/DiveInoScreen.h"
#include "Preferences.h"

#include "Arduino.h"
#include "M5ez.h"
#include "ezTime.h"

bool DiveInoScreen::isClockWidgetDisplayed()
{
  Preferences prefs;
  prefs.begin("M5ez", true); // read-only
  bool isDisplayed = prefs.getBool("clock_on", true);
  prefs.end();
  return isDisplayed;
}

void DiveInoScreen::refreshClockWidget()
{
  //HACK: Refresh the clock widget at the top in every minute
  if (this->isClockWidgetDisplayed())
  {
    ez.header.draw("clock");
  }
}