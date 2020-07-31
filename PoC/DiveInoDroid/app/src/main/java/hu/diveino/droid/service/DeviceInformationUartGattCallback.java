package hu.diveino.droid.service;

import android.os.Handler;
import android.util.Log;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import hu.diveino.droid.ble.UartGattCallback;
import hu.diveino.droid.model.json.Logbook;
import hu.diveino.droid.model.json.Profile;
import hu.diveino.droid.model.json.ProfileItem;
import hu.diveino.droid.model.json.ProfileSummary;

public class DeviceInformationUartGattCallback extends UartGattCallback {

    private static final String TAG = "DeviceInfoCallback";

    public static final int MESSAGE_LOGBOOK_INFO_COMPLETED = 11;
    public static final int MESSAGE_PROFILE_INFO_COMPLETED = 21;

    public DeviceInformationUartGattCallback(Handler replyMessageHandler) {
        super(replyMessageHandler);
    }

    @Override
    public Object parseResponseValue(String responseValue) {
        Object parsedValue = responseValue;
        switch (this.getMessageType()) {
            case LOGBOOK_INFO: {
                parsedValue = JsonParser.parseLogbook(responseValue);
            } break;
            case LOGBOOK_PROFILE_INFO: {
                parsedValue = JsonParser.parseProfile(responseValue);
            } break;
        }
        return parsedValue;
    }

    @Override
    public int getResponseMessageType() {
        switch (this.getMessageType()) {
            case LOGBOOK_INFO:
                return MESSAGE_LOGBOOK_INFO_COMPLETED;
            case LOGBOOK_PROFILE_INFO:
                return MESSAGE_PROFILE_INFO_COMPLETED;
        }
        return UartGattCallback.MESSAGE_ERROR_COMPLETED;
    }

}
