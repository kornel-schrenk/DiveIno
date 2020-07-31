package hu.diveino.droid.service;

import android.util.Log;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import hu.diveino.droid.model.json.Logbook;
import hu.diveino.droid.model.json.Profile;
import hu.diveino.droid.model.json.ProfileItem;
import hu.diveino.droid.model.json.ProfileSummary;
import hu.diveino.droid.model.json.Settings;

public final class JsonParser {

    private static final String TAG = "JsonParser";

    private JsonParser(){}

    public static Logbook parseLogbook(String jsonContent) {
        Logbook logbook = new Logbook();
        try {
            JSONObject logbookSummaryRoot = new JSONObject(jsonContent);
            logbook.setNumberOfDives(logbookSummaryRoot.getInt("numberOfDives"));
            logbook.setLoggedDiveHours(logbookSummaryRoot.getInt("loggedDiveHours"));
            logbook.setLoggedDiveMinutes(logbookSummaryRoot.getInt("loggedDiveMinutes"));
            logbook.setMaxDepth(logbookSummaryRoot.getDouble("maxDepth"));
            logbook.setLastDiveDate(logbookSummaryRoot.getString("lastDiveDate"));
            logbook.setLastDiveTime(logbookSummaryRoot.getString("lastDiveTime"));
            logbook.setNumberOfStoredProfiles(logbookSummaryRoot.getInt("numberOfStoredProfiles"));
        } catch (JSONException e) {
            Log.e(TAG, jsonContent, e);
        }
        return logbook;
    }

    public static Profile parseProfile(String jsonContent) {
        Profile profile = new Profile();
        try {
            JSONObject profileRoot = new JSONObject(jsonContent);

            JSONObject summaryRoot = profileRoot.getJSONObject("summary");
            ProfileSummary profileSummary = new ProfileSummary();
            profileSummary.setDiveDuration(summaryRoot.getInt("diveDuration"));
            profileSummary.setMaxDepth(summaryRoot.getDouble("maxDepth"));
            profileSummary.setMinTemperature(summaryRoot.getDouble("minTemperature"));
            profileSummary.setOxygenPercentage(summaryRoot.getDouble("oxygenPercentage"));
            profileSummary.setDiveDate(summaryRoot.getString("diveDate"));
            profileSummary.setDiveTime(summaryRoot.getString("diveTime"));
            profile.setProfileSummary(profileSummary);

            JSONArray itemsRoot = profileRoot.getJSONArray("profile");
            for (int i = 0; i < itemsRoot.length(); i++) {
                JSONObject item = itemsRoot.getJSONObject(i);
                ProfileItem profileItem = new ProfileItem();
                profileItem.setDepth(item.getDouble("depth"));
                profileItem.setPressure(item.getDouble("pressure"));
                profileItem.setDuration(item.getInt("duration"));
                profileItem.setTemperature(item.getDouble("temperature"));
                profile.getProfileItems().add(profileItem);
            }
        } catch (JSONException e) {
            Log.e(TAG, jsonContent, e);
        }
        return profile;
    }

    public static Settings parseSettings(String jsonContent) {
        Settings settings = new Settings();
        try {
            JSONObject settingsRoot = new JSONObject(jsonContent);
            settings.setSeaLevelPressure(settingsRoot.getDouble("seaLevelPressure"));
            settings.setOxygenPercentage(settingsRoot.getDouble("oxygenPercentage"));
            settings.setSoundEnabled(settingsRoot.getBoolean("soundEnabled"));
            settings.setImperialUnits(settingsRoot.getBoolean("isImperialUnits"));
        } catch (JSONException e) {
            Log.e(TAG, jsonContent, e);
        }
        return settings;
    }
}
