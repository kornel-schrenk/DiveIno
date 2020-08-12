#include "screens/MainMenu.h"

ezMenu MainMenu::initMainMenu()
{
  ezMenu mainMenu = ezMenu("", true);
  mainMenu.buttons("left # Select # right");

  //If the image is 96x96 px
  mainMenu.imgFromTop(72);
  mainMenu.imgFromLeft(112);
  
  if (ez.theme->name == "Default") {  
    mainMenu.imgBackground(ez.theme->background);
    mainMenu.imgCaptionColor(ez.theme->header_bgcolor);

    //JPG files from FLASH - Default theme
    mainMenu.addItem(dive_jpg, "Dive");
    mainMenu.addItem(gauge_jpg, "Gauge");
    mainMenu.addItem(logbook_jpg, "Logbook");
    mainMenu.addItem(surface_jpg, "Surface");
    mainMenu.addItem(settings_jpg, "Settings");
    mainMenu.addItem(about_jpg, "About");
    mainMenu.addItem(back_jpg, "Back");
  } else if (ez.theme->name == "Dark") {
    mainMenu.imgBackground(ez.theme->background);
    mainMenu.imgCaptionColor(ez.theme->header_fgcolor);

    //JPG files from FLASH - Dark theme
    mainMenu.addItem(dive_jpg_dark, "Dive");
    mainMenu.addItem(gauge_jpg_dark, "Gauge");
    mainMenu.addItem(logbook_jpg_dark, "Logbook");
    mainMenu.addItem(surface_jpg_dark, "Surface");
    mainMenu.addItem(settings_jpg_dark, "Settings");
    mainMenu.addItem(about_jpg_dark, "About");
    mainMenu.addItem(back_jpg_dark, "Back");
  }
  
  return mainMenu;
}