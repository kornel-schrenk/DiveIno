package hu.diveino.droid.service;

import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.os.Handler;
import android.util.Log;

import java.nio.charset.Charset;

import hu.diveino.droid.ble.UartGattCallback;
import hu.diveino.droid.util.ByteUtils;

public class DiagnosticsUartGattCallback extends UartGattCallback {

    private static final String TAG = "DiagnosticsCallback";

    public static final int MESSAGE_DIAG_TEMP_COMPLETED = 10;
    public static final int MESSAGE_DIAG_PRES_COMPLETED = 20;
    public static final int MESSAGE_DIAG_BAT_COMPLETED = 30;
    public static final int MESSAGE_DIAG_DATE_COMPLETED = 40;
    public static final int MESSAGE_DIAG_TIMESTAMP_COMPLETED = 50;

    public static final int MESSAGE_GENERAL_VERSION_COMPLETED = 60;
    public static final int MESSAGE_GENERAL_RESET_COMPLETED = 70;
    public static final int MESSAGE_GENERAL_CLEAR_SURFACE_COMPLETED = 80;

    public DiagnosticsUartGattCallback(Handler replyMessageHandler) {
        super(replyMessageHandler);
    }

    @Override
    public Object parseResponseValue(String responseValue) {
        Object parsedValue = responseValue;
        switch (this.getMessageType()) {
            case DIAG_TEMP: {
                Double temperature = 0.0;
                try {
                    temperature = Double.valueOf(responseValue);
                } catch (NumberFormatException nfe) {
                    Log.e(TAG, "Unable to parse INFO DIAG TEMP message reply: " + responseValue);
                }
                Log.i(TAG, "Parsed INFO DIAG TEMP message reply: " + temperature);
                parsedValue = temperature;
            }
            break;
            case DIAG_PRES: {
                Double pressure = 0.0;
                try {
                    pressure = Double.valueOf(responseValue);
                } catch (NumberFormatException nfe) {
                    Log.e(TAG, "Unable to parse INFO DIAG PRES message reply: " + responseValue);
                }
                Log.i(TAG, "Parsed INFO DIAG PRES message reply: " + pressure);
                parsedValue = pressure;
            } break;
            case DIAG_BAT: {
                Double batteryCharge = 0.0;
                try {
                    batteryCharge = Double.valueOf(responseValue);
                } catch (NumberFormatException nfe) {
                    Log.e(TAG, "Unable to parse INFO DIAG BAT message reply: " + responseValue);
                }
                Log.i(TAG, "Parsed INFO DIAG BAT message reply: " + batteryCharge);
                parsedValue = batteryCharge;
            } break;
        }
        return parsedValue;
    }

    @Override
    public int getResponseMessageType() {
        switch (this.getMessageType()) {
            case DIAG_TEMP:
                return MESSAGE_DIAG_TEMP_COMPLETED;
            case DIAG_PRES:
                return MESSAGE_DIAG_PRES_COMPLETED;
            case DIAG_BAT:
                return MESSAGE_DIAG_BAT_COMPLETED;
            case DIAG_DATE:
                return MESSAGE_DIAG_DATE_COMPLETED;
            case DIAG_TIMESTAMP:
                return MESSAGE_DIAG_TIMESTAMP_COMPLETED;
            case GENERAL_VERSION:
                return MESSAGE_GENERAL_VERSION_COMPLETED;
            case GENERAL_RESET:
                return MESSAGE_GENERAL_RESET_COMPLETED;
            case GENERAL_CLEAR_SURFACE:
                return MESSAGE_GENERAL_CLEAR_SURFACE_COMPLETED;
        }
        return UartGattCallback.MESSAGE_ERROR_COMPLETED;
    }
}
