#include "screens/SurfaceScreen.h"

void SurfaceScreen::init(DiveInoSettings diveInoSettings, DiveResult* diveResult)
{
    _diveInoSettings = diveInoSettings;
    _diveResult = diveResult;

    ez.screen.clear();
    ez.header.show("Surface");

    ez.buttons.show("Menu");

    _display();
}

void SurfaceScreen::_display()
{
    
    if (ez.theme->name == "Default") { 
        M5.Lcd.drawJpg((uint8_t *)ok_small_jpg, (sizeof(ok_small_jpg) / sizeof(ok_small_jpg[0])), 105, 35, 32, 32);
        M5.Lcd.drawJpg((uint8_t *)airplane_small_jpg, (sizeof(airplane_small_jpg) / sizeof(airplane_small_jpg[0])), 105, 75, 32, 32);
        M5.Lcd.drawJpg((uint8_t *)max_depth_small_jpg, (sizeof(max_depth_small_jpg) / sizeof(max_depth_small_jpg[0])), 105, 115, 32, 32);
        M5.Lcd.drawJpg((uint8_t *)deco_stop_small_jpg, (sizeof(deco_stop_small_jpg) / sizeof(deco_stop_small_jpg[0])), 105, 155, 32, 32);
    }
    if (ez.theme->name == "Dark") {
        M5.Lcd.drawJpg((uint8_t *)ok_small_jpg_dark, (sizeof(ok_small_jpg_dark) / sizeof(ok_small_jpg_dark[0])), 105, 35, 32, 32);
        M5.Lcd.drawJpg((uint8_t *)airplane_small_jpg, (sizeof(airplane_small_jpg) / sizeof(airplane_small_jpg[0])), 105, 75, 32, 32);
        M5.Lcd.drawJpg((uint8_t *)max_depth_small_jpg, (sizeof(max_depth_small_jpg) / sizeof(max_depth_small_jpg[0])), 105, 115, 32, 32);
        M5.Lcd.drawJpg((uint8_t *)deco_stop_small_jpg_dark, (sizeof(deco_stop_small_jpg_dark) / sizeof(deco_stop_small_jpg_dark[0])), 105, 155, 32, 32);
    }

    ez.canvas.color(ez.theme->foreground);
    ez.canvas.font(sans26);

    if (_diveResult != NULL) {
        ez.canvas.pos(150, 40);
        ez.canvas.print(zeropad(_diveResult->durationInSeconds/60, 2)); //minutes
        ez.canvas.print(":");
        ez.canvas.print(zeropad(_diveResult->durationInSeconds%60, 2)); //seconds

        ez.canvas.pos(150, 80);        
        ez.canvas.print(zeropad(_diveResult->noFlyTimeInMinutes/60, 2)); //hours
        ez.canvas.print(":");
        ez.canvas.print(zeropad(_diveResult->noFlyTimeInMinutes%60, 2)); //minutes        

        ez.canvas.pos(150, 120);
        if (_diveInoSettings.imperialUnitsSetting) {
            ez.canvas.print(_diveResult->maxDepthInMeters*3.28, 0);
            ez.canvas.print(" ft");
        } else {
            ez.canvas.print(_diveResult->maxDepthInMeters);
            ez.canvas.print(" m");
        }        

        ez.canvas.pos(150, 160);
        if (_diveResult->wasDecoDive) {
            ez.canvas.print("yes");
        } else {
            ez.canvas.print("no");
        }        
    } else {
        ez.canvas.pos(150, 40);
        ez.canvas.print("00:00");

        ez.canvas.pos(150, 80);
        ez.canvas.print("00:00");

        ez.canvas.pos(150, 120);
        ez.canvas.print("n/a");

        ez.canvas.pos(150, 160);
        ez.canvas.print("n/a");
    }
}

void SurfaceScreen::handleButtonPress(String buttonName)
{
}