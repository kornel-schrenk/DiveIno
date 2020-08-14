#include "screens/GaugeScreen.h"

void GaugeScreen::refreshSensorData(PressureSensorData sensorData)
{
    ez.canvas.color(ez.theme->foreground);
    ez.canvas.font(sans26);

    ez.canvas.pos(50, 50);
    ez.canvas.print(sensorData.pressureInMillibar, 0);

    ez.canvas.pos(50, 120);
    ez.canvas.print(sensorData.temperatureInCelsius, 0);
}

void GaugeScreen::init(PressureSensorData sensorData)
{
    ez.screen.clear();
    ez.header.show("Gauge");
    ez.buttons.show("# Menu #");

    refreshSensorData(sensorData);
}

void GaugeScreen::display(PressureSensorData sensorData)
{
    refreshSensorData(sensorData);
}

void GaugeScreen::handleButtonPress(String buttonName)
{
}