package hu.diveino.droid.service;

import android.os.Handler;
import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import hu.diveino.droid.ble.UartGattCallback;
import hu.diveino.droid.model.json.Settings;

public class ConfigurationUartGattCallback extends UartGattCallback {

    private static final String TAG = "DeviceInfoCallback";

    public static final int SETTINGS_INFO_COMPLETED = 12;
    public static final int SETTINGS_DEFAULT_COMPLETED = 22;
    public static final int SETTINGS_SOUND_COMPLETED = 32;
    public static final int SETTINGS_METRIC_COMPLETED = 42;
    public static final int SETTINGS_PRESSURE_COMPLETED = 52;
    public static final int SETTINGS_OXYGEN_COMPLETED = 62;
    public static final int SETTINGS_DATETIME_COMPLETED = 72;
    public static final int SETTINGS_AUTO_COMPLETED = 82;

    public ConfigurationUartGattCallback(Handler replyMessageHandler) {
        super(replyMessageHandler);
    }

    @Override
    public Object parseResponseValue(String responseValue) {
        Object parsedValue = responseValue;
        switch (this.getMessageType()) {
            case SETTINGS_INFO: {
                parsedValue = JsonParser.parseSettings(responseValue);
            } break;
            case SETTINGS_DEFAULT: {
                parsedValue = new Settings();
            } break;
            case SETTINGS_SOUND: {
                parsedValue = Boolean.TRUE;
                if (responseValue.contains("OFF")) {
                    parsedValue = Boolean.FALSE;
                }
            } break;
            case SETTINGS_METRIC: {
                parsedValue = Boolean.FALSE;
                if (responseValue.contains("OFF")) {
                    parsedValue = Boolean.TRUE;
                }
            } break;
            case SETTINGS_PRESSURE: {
                Double pressure = 1013.2;
                if (responseValue.contains("PRESSURE")) {
                    String value = responseValue.substring(responseValue.indexOf('-') + 2);
                    try {
                        pressure = Double.parseDouble(value);
                    } catch (NumberFormatException nfe) {
                        Log.w(TAG, "Passed back pressure is not a number: " + value);
                    }
                }
                parsedValue = pressure;
            } break;
            case SETTINGS_OXYGEN: {
                Double oxygenPercentage = 0.21;
                if (responseValue.contains("OXYGEN")) {
                    String percentage = responseValue.substring(responseValue.indexOf('-') + 2);
                    try {
                        oxygenPercentage = Double.parseDouble(percentage);
                    } catch (NumberFormatException nfe) {
                        Log.w(TAG, "Passed back oxygen percentage is not a number: " + percentage);
                    }
                }
                parsedValue = oxygenPercentage;
            } break;
            case SETTINGS_DATETIME: {
                String dateTimeText = "";
                if (responseValue.contains("DATETIME")) {
                    dateTimeText = responseValue.substring(responseValue.indexOf('-') + 2);
                }
                parsedValue = dateTimeText;
            } break;
            case SETTINGS_AUTO: {
                Double pressure = 1013.2;
                if (responseValue.contains("AUTO")) {
                    String value = responseValue.substring(responseValue.indexOf('-') + 2);
                    try {
                        pressure = Double.parseDouble(value);
                    } catch (NumberFormatException nfe) {
                        Log.w(TAG, "Passed back pressure is not a number: " + value);
                    }
                }
                parsedValue = pressure;
            } break;

        }
        return parsedValue;
    }

    @Override
    public int getResponseMessageType() {
        switch (this.getMessageType()) {
            case SETTINGS_INFO:
                return SETTINGS_INFO_COMPLETED;
            case SETTINGS_DEFAULT:
                return SETTINGS_DEFAULT_COMPLETED;
            case SETTINGS_SOUND:
                return SETTINGS_SOUND_COMPLETED;
            case SETTINGS_METRIC:
                return SETTINGS_METRIC_COMPLETED;
            case SETTINGS_PRESSURE:
                return SETTINGS_PRESSURE_COMPLETED;
            case SETTINGS_OXYGEN:
                return SETTINGS_OXYGEN_COMPLETED;
            case SETTINGS_DATETIME:
                return SETTINGS_DATETIME_COMPLETED;
            case SETTINGS_AUTO:
                return SETTINGS_AUTO_COMPLETED;
        }
        return UartGattCallback.MESSAGE_ERROR_COMPLETED;
    }
}
