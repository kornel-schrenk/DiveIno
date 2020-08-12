#include "pickers/SettingsPicker.h"
#include <Preferences.h>

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