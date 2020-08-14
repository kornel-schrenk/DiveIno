#include "pickers/SettingsPicker.h"
#include <Preferences.h>

DiveInoSettings SettingsPicker::getDiveInoSettings()
{
    struct DiveInoSettings diveInoSettings;

    Preferences prefs;
    prefs.begin("DiveIno", true);	// read-only
    diveInoSettings.seaLevelPressureSetting = prefs.getInt("pressure", 1013);
    diveInoSettings.oxygenRateSetting = prefs.getFloat("oxygen", 0.21);
    diveInoSettings.soundSetting = prefs.getBool("sound", false);
    diveInoSettings.imperialUnitsSetting = prefs.getBool("imperial", false);
    prefs.end();

    return diveInoSettings;    
}

///////////////////////////
// Variable declarations //
///////////////////////////

// General picker settings
String SettingsPicker::_pickerButtons;
GFXfont SettingsPicker::_pickerFont;
int8_t SettingsPicker::_pickerCheckType;
String SettingsPicker::_pickerCheckButtonName;

/////////////
// Pickers //
/////////////

// Wifi Settings //

void SettingsPicker::_displayWifiPicker()
{
    ez.wifi.menu();
}

bool SettingsPicker::_advancedDisplayWifiPicker(ezMenu *callingMenu)
{
    if (callingMenu->pickButton() != "Ok")
    {
        _displayWifiPicker();
    }
    return true;
}

// Clock Settings //

void SettingsPicker::_displayClockPicker()
{
    ez.clock.menu();
}

bool SettingsPicker::_advancedDisplayClockPicker(ezMenu *callingMenu)
{
    if (callingMenu->pickButton() != "Ok")
    {
        _displayClockPicker();
    }
    return true;
}

// Battery Settings //

void SettingsPicker::_displayBatteryPicker()
{
    ez.battery.menu();
}

bool SettingsPicker::_advancedDisplayBatteryPicker(ezMenu *callingMenu)
{
    if (callingMenu->pickButton() != "Ok")
    {
        _displayBatteryPicker();
    }
    return true;
}

// Backlight Settings //

void SettingsPicker::_displayBacklightPicker()
{
    uint8_t _brightness = 128;
    
    Preferences prefs;
    prefs.begin("M5ez", true);	// read-only
    _brightness = prefs.getUChar("brightness", 128);
    prefs.end();

    uint8_t _startBrightness = _brightness;

    ezProgressBar bl ("Backlight", "Brightness:", "left#Ok|Select#right");
    while (true) {
        String b = ez.buttons.poll();
        if (b == "right" && _brightness <= 240) _brightness += 16;
        if (!_brightness) _brightness = 255;
        if (b == "left" && _brightness > 16) _brightness -= 16;
        if (_brightness == 239) _brightness = 240;
        bl.value((float)(_brightness / 2.55));
        m5.lcd.setBrightness(_brightness);
        if (b == "Ok") break;
    }

    if (_brightness != _startBrightness) {
        Preferences prefs;
        prefs.begin("M5ez");
        prefs.putUChar("brightness", _brightness);
        prefs.end();
    }
}

bool SettingsPicker::_advancedDisplayBacklightPicker(ezMenu *callingMenu)
{
    if (callingMenu->pickButton() != "Ok")
    {
        _displayBacklightPicker();
    }
    return true;
}

// Theme Settings //

void SettingsPicker::_displayThemePicker()
{
    String originalThemeName = ez.theme->name;

    ezMenu themePickerMenu("Theme");

    themePickerMenu.buttons(_pickerButtons);
    themePickerMenu.txtFont(ez.theme -> menu_small_font);
    themePickerMenu.setCheckType(_pickerCheckType);
    themePickerMenu.setCheckButtonName(_pickerCheckButtonName);

    for (uint8_t n = 0; n < ez.themes.size(); n++)
    {
        themePickerMenu.addItem(ez.themes[n].name);
    }

    themePickerMenu.runOnce();

    if (themePickerMenu.pickButton() == "Ok")
    {
        ez.theme->select(themePickerMenu.pickName());

        if (ez.theme->name != originalThemeName)
        {
            Preferences prefs;
            prefs.begin("M5ez");
            prefs.putString("theme", ez.theme->name);
            prefs.end();
        }
    }
}

bool SettingsPicker::_advancedDisplayThemePicker(ezMenu *callingMenu)
{
    if (callingMenu->pickButton() != "Ok")
    {
        _displayThemePicker();
    }
    return true;
}

// Factory Defaults Settings //

void SettingsPicker::_displayFactoryDefaultsPicker()
{
    if (ez.msgBox("Factory reset", "Are you sure you want to reset all settings to factory defaults?", "Yes##No") == "Yes")
    {
        Preferences prefs;
        prefs.begin("M5ez");
        prefs.clear();
        prefs.end();
        ESP.restart();
    }
}

bool SettingsPicker::_advancedDisplayFactoryDefaultsPicker(ezMenu *callingMenu)
{
    if (callingMenu->pickButton() != "Ok")
    {
        _displayFactoryDefaultsPicker();
    }
    return true;
}

// Sound Settings //

void SettingsPicker::_displaySoundPicker()
{
    DiveInoSettings diveInoSettings = getDiveInoSettings();
	bool originalSetting = diveInoSettings.soundSetting;
    while(true) {
        ezMenu soundSettingMenu("Sound");
        soundSettingMenu.txtSmall();
        soundSettingMenu.buttons("up#Back#select##down#");
        soundSettingMenu.addItem("on|Sound \t" + (String)(diveInoSettings.soundSetting ? "on" : "off"));
        switch (soundSettingMenu.runOnce()) {
            case 1: //Select pressed
                diveInoSettings.soundSetting = !diveInoSettings.soundSetting;
                break;
            case 0: //Back pressed
                if (diveInoSettings.soundSetting != originalSetting) {                    
                    //Save the modified sound setting
                    Preferences prefs;
                    prefs.begin("DiveIno");
                    prefs.putBool("sound", diveInoSettings.soundSetting);
                    prefs.end();
                }
                return;
        }
    }    
}

bool SettingsPicker::_advancedDisplaySoundPicker(ezMenu *callingMenu)
{
    if (callingMenu->pickButton() != "Ok")
    {
        _displaySoundPicker();
    }
    return true;
}

// Imperial Units Selection Settings //

void SettingsPicker::_displayImperialUnitsPicker()
{
    DiveInoSettings diveInoSettings = getDiveInoSettings();
	bool originalSetting = diveInoSettings.imperialUnitsSetting;
    while(true) {
        ezMenu imperialUnitsSettingMenu("Imperial units");
        imperialUnitsSettingMenu.txtSmall();
        imperialUnitsSettingMenu.buttons("up#Back#select##down#");
        imperialUnitsSettingMenu.addItem("on|Imperial units \t" + (String)(diveInoSettings.imperialUnitsSetting ? "on" : "off"));
        switch (imperialUnitsSettingMenu.runOnce()) {
            case 1: //Select pressed
                diveInoSettings.imperialUnitsSetting = !diveInoSettings.imperialUnitsSetting;
                break;
            case 0: //Back pressed
                if (diveInoSettings.imperialUnitsSetting != originalSetting) {
                    //Save the modified imperial units setting
                    Preferences prefs;
                    prefs.begin("DiveIno");
                    prefs.putBool("imperial", diveInoSettings.imperialUnitsSetting);
                    prefs.end();
                }
                return;
        }
    }        
}

bool SettingsPicker::_advancedDisplayImperialUnitsPicker(ezMenu *callingMenu)
{
    if (callingMenu->pickButton() != "Ok")
    {
        _displayImperialUnitsPicker();
    }
    return true;
}

// Oxygen Rate Setting //

void SettingsPicker::_displayOxygenRatePicker()
{
    DiveInoSettings diveInoSettings = getDiveInoSettings();
	uint8_t oxygenRateInt = diveInoSettings.oxygenRateSetting * 100;
    uint8_t originalOxygenRateInt = diveInoSettings.oxygenRateSetting * 100;

    ezProgressBar progressBar ("Oxygen rate", "", "left#Ok|Select#right");

    if (ez.theme->name == "Default") {      
      M5.Lcd.drawJpg((uint8_t *)oxygen_jpg, (sizeof(oxygen_jpg) / sizeof(oxygen_jpg[0])), 88, 40, 64, 64);  
    } else {      
      M5.Lcd.drawJpg((uint8_t *)oxygen_jpg_dark, (sizeof(oxygen_jpg_dark) / sizeof(oxygen_jpg_dark[0])), 88, 40, 64, 64);  
    }  
    
    while (true) {
        String button = ez.buttons.poll();
        if (button == "right" && oxygenRateInt < 40) oxygenRateInt++;            
        if (button == "left" && oxygenRateInt > 21) oxygenRateInt--;        
        
        float progressBarValue = (oxygenRateInt - 21) / 0.19;
        progressBar.value(progressBarValue);

        ez.canvas.color(ez.theme->foreground);
        ez.canvas.font(sans26);

        ez.canvas.pos(180, 60);
        ez.canvas.print(oxygenRateInt);
        ez.canvas.pos(210, 60);
        ez.canvas.print("%");

        if (button == "Ok") break;
    }

    if (oxygenRateInt != originalOxygenRateInt) {
        diveInoSettings.oxygenRateSetting = (float)oxygenRateInt / 100; 

        //Save the modified Oxygen Rate setting
        Preferences prefs;
        prefs.begin("DiveIno");
        prefs.putFloat("oxygen", diveInoSettings.oxygenRateSetting);
        prefs.end();
    }    
}

bool SettingsPicker::_advancedDisplayOxygenRatePicker(ezMenu *callingMenu)
{
    if (callingMenu->pickButton() != "Ok")
    {
        _displayOxygenRatePicker();
    }
    return true;
}

// Sea Level Pressure Setting //

void SettingsPicker::_displaySeaLevelPressurePicker()
{
    DiveInoSettings diveInoSettings = getDiveInoSettings();
	int pressureRateInt = diveInoSettings.seaLevelPressureSetting;
    int originalPressureRateInt = diveInoSettings.seaLevelPressureSetting;

    ezProgressBar progressBar ("Sea level pressure", "", "left#Ok|Select#right");

    if (ez.theme->name == "Default") {      
      M5.Lcd.drawJpg((uint8_t *)pressure_jpg, (sizeof(pressure_jpg) / sizeof(pressure_jpg[0])), 88, 40, 64, 64);  
    } else {      
      M5.Lcd.drawJpg((uint8_t *)pressure_jpg_dark, (sizeof(pressure_jpg_dark) / sizeof(pressure_jpg_dark[0])), 88, 40, 64, 64);  
    }  
    
    while (true) {
        String button = ez.buttons.poll();
        if (button == "right" && pressureRateInt < 1030) pressureRateInt++;            
        if (button == "left" && pressureRateInt > 990) pressureRateInt--;        
        
        float progressBarValue = (pressureRateInt - 990) / 0.6;
        progressBar.value(progressBarValue);

        ez.canvas.color(ez.theme->foreground);
        ez.canvas.font(sans26);

        ez.canvas.pos(180, 60);
        ez.canvas.print(pressureRateInt);
        ez.canvas.pos(250, 60);
        ez.canvas.print("mbar");

        if (button == "Ok") break;
    }

    if (pressureRateInt != originalPressureRateInt) {
        diveInoSettings.seaLevelPressureSetting = pressureRateInt; 

        //Save the modified Sea Level Pressure setting
        Preferences prefs;
        prefs.begin("DiveIno");
        prefs.putInt("pressure", diveInoSettings.seaLevelPressureSetting);
        prefs.end();
    }   
}

bool SettingsPicker::_advancedDisplaySeaLevelPressurePicker(ezMenu* callingMenu)
{
    if (callingMenu->pickButton() != "Ok")
    {
        _displaySeaLevelPressurePicker();
    }
    return true;
}

/////////////////
// Picker Menu //
/////////////////

String SettingsPicker::_displayPickerMenu(String pickerName)
{
    ezMenu pickerMenu(pickerName);

    pickerMenu.buttons("up##Select##down#Back|Menu");    
    pickerMenu.txtFont(ez.theme -> menu_small_font);

    // Advanced functions are required, because the "Ok" button is reserved for saving and "Select" button is used to reach sub-level picker menus
    // In M5ez all non navigation buttons are action buttons
    pickerMenu.addItem("Wifi", NULL, _advancedDisplayWifiPicker, NULL);    
    pickerMenu.addItem("Clock", NULL, _advancedDisplayClockPicker, NULL);
    pickerMenu.addItem("Battery", NULL, _advancedDisplayBatteryPicker, NULL);
    pickerMenu.addItem("Backlight", NULL, _advancedDisplayBacklightPicker, NULL);    
    if (ez.themes.size() > 1)
    {
        pickerMenu.addItem("Theme Chooser", NULL, _advancedDisplayThemePicker, NULL);
    }
    pickerMenu.addItem("Factory Defaults", NULL, _advancedDisplayFactoryDefaultsPicker, NULL);
    pickerMenu.addItem("Sea level pressure", NULL, _advancedDisplaySeaLevelPressurePicker, NULL);
    pickerMenu.addItem("Oxygen rate", NULL, _advancedDisplayOxygenRatePicker, NULL);
    pickerMenu.addItem("Sound", NULL, _advancedDisplaySoundPicker, NULL);
    pickerMenu.addItem("Imperial units", NULL, _advancedDisplayImperialUnitsPicker, NULL);

    pickerMenu.runOnce();

    if (pickerMenu.pickButton() == "Back")
    {
        return "Back";
    }
    else if (pickerMenu.pickButton() == "Ok")
    {
        return "Ok";
    }
    return "";
}

void SettingsPicker::runOnce(String pickerName)
{
    _pickerButtons = "up##Ok|Select##down#Back";
    _pickerFont = FreeMonoBold12pt7b;
    _pickerCheckType = CHECK_TPYE_NONE;
    _pickerCheckButtonName = "Select";

    ez.battery.setMenuButtons(_pickerButtons);
    ez.battery.setMenuHeader("Battery");

    ez.clock.setMenuButtons(_pickerButtons);
    ez.clock.setMenuHeader("Clock");

    ez.wifi.setMenuButtons(_pickerButtons);
    ez.wifi.setMenuHeader("Wifi");

    while (true)
    {
        String selectedAction = _displayPickerMenu(pickerName);
        if (selectedAction == "Back")
        {
            return;
        }
        else if (selectedAction == "Ok")
        {
            break;
        }
    }
}