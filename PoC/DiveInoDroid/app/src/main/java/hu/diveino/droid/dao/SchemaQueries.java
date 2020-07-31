package hu.diveino.droid.dao;

import android.content.ContentValues;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.util.Log;

import hu.diveino.droid.model.database.DiveProfileEntry;
import hu.diveino.droid.model.database.LogbookEntry;
import hu.diveino.droid.model.json.Profile;
import hu.diveino.droid.model.json.ProfileItem;
import hu.diveino.droid.model.json.ProfileSummary;

public final class SchemaQueries {

    private static final String TAG = "SchemaQueries";

    private SchemaQueries() {}

    static final String SQL_CREATE_DIVE_PROFILE =
            "CREATE TABLE " + SchemaContract.DiveProfileTable.TABLE_NAME + " (" +
                    SchemaContract.DiveProfileTable._ID + " INTEGER PRIMARY KEY, " +
                    SchemaContract.DiveProfileTable.COLUMN_NAME_DIVE_DURATION + " INTEGER NOT NULL, " +
                    SchemaContract.DiveProfileTable.COLUMN_NAME_MAX_DEPTH + " REAL NOT NULL, " +
                    SchemaContract.DiveProfileTable.COLUMN_NAME_MIN_TEMPERATURE + " REAL, " +
                    SchemaContract.DiveProfileTable.COLUMN_NAME_OXYGEN_PERCENTAGE + " REAL NOT NULL, " +
                    SchemaContract.DiveProfileTable.COLUMN_NAME_DIVE_DATE_TIME + " TEXT NOT NULL)";

    static final String SQL_DELETE_DIVE_PROFILE =
            "DROP TABLE IF EXISTS " + SchemaContract.DiveProfileTable.TABLE_NAME;

    static final String SQL_CREATE_PROFILE_ITEM =
            "CREATE TABLE " + SchemaContract.ProfileItemTable.TABLE_NAME + " (" +
                    SchemaContract.ProfileItemTable._ID + " INTEGER PRIMARY KEY, " +
                    SchemaContract.ProfileItemTable.COLUMN_NAME_DIVE_PROFILE_ID + " INTEGER NOT NULL, " +
                    SchemaContract.ProfileItemTable.COLUMN_NAME_DEPTH + " REAL NOT NULL, " +
                    SchemaContract.ProfileItemTable.COLUMN_NAME_PRESSURE + " REAL NOT NULL, " +
                    SchemaContract.ProfileItemTable.COLUMN_NAME_DURATION + " INTEGER NOT NULL, " +
                    SchemaContract.ProfileItemTable.COLUMN_NAME_TEMPERATURE + " REAL, " +
                    "FOREIGN KEY(" + SchemaContract.ProfileItemTable.COLUMN_NAME_DIVE_PROFILE_ID + ") REFERENCES " +
                    SchemaContract.DiveProfileTable.TABLE_NAME + "(" + SchemaContract.DiveProfileTable._ID + "))";

    static final String SQL_DELETE_PROFILE_ITEM =
            "DROP TABLE IF EXISTS " + SchemaContract.ProfileItemTable.TABLE_NAME;

    /////////////
    // Queries //
    /////////////

    public static LogbookEntry getLogbook(SQLiteDatabase db) {
        try (Cursor profileCursor = SchemaQueries.getDiveProfiles(db)) {
            return SchemaMapper.mapDiveProfilesToLogbookEntry(profileCursor);
        }
    }

    public static boolean addDiveProfile(SQLiteDatabase db, Profile diveProfile) {

        boolean success = false;

        ProfileSummary diveProfileSummary = diveProfile.getProfileSummary();

        //FIRST - Create a new entry in the DIVE_PROFILE table
        ContentValues diveProfileValues = new ContentValues();
        diveProfileValues.put(SchemaContract.DiveProfileTable.COLUMN_NAME_DIVE_DURATION, diveProfileSummary.getDiveDuration());
        diveProfileValues.put(SchemaContract.DiveProfileTable.COLUMN_NAME_MAX_DEPTH, diveProfileSummary.getMaxDepth());
        diveProfileValues.put(SchemaContract.DiveProfileTable.COLUMN_NAME_MIN_TEMPERATURE, diveProfileSummary.getMinTemperature());
        diveProfileValues.put(SchemaContract.DiveProfileTable.COLUMN_NAME_OXYGEN_PERCENTAGE, diveProfileSummary.getOxygenPercentage());
        diveProfileValues.put(SchemaContract.DiveProfileTable.COLUMN_NAME_DIVE_DATE_TIME, diveProfileSummary.getDiveDate() + " " + diveProfileSummary.getDiveTime());

        Long diveProfileId = db.insert(SchemaContract.DiveProfileTable.TABLE_NAME, null, diveProfileValues);
        if (-1L != diveProfileId) {
            success = true;

            //SECOND - Create dive profile entries into the PROFILE_ITEM table
            for (ProfileItem profileItem : diveProfile.getProfileItems()) {
                ContentValues profileValues = new ContentValues();
                profileValues.put(SchemaContract.ProfileItemTable.COLUMN_NAME_DIVE_PROFILE_ID, diveProfileId);
                profileValues.put(SchemaContract.ProfileItemTable.COLUMN_NAME_DEPTH, profileItem.getDepth());
                profileValues.put(SchemaContract.ProfileItemTable.COLUMN_NAME_PRESSURE, profileItem.getPressure());
                profileValues.put(SchemaContract.ProfileItemTable.COLUMN_NAME_DURATION, profileItem.getDuration());
                profileValues.put(SchemaContract.ProfileItemTable.COLUMN_NAME_TEMPERATURE, profileItem.getTemperature());

                Long profileItemId = db.insert(SchemaContract.ProfileItemTable.TABLE_NAME, null, profileValues);
                if (-1L == profileItemId) {
                    success = false;
                    Log.e(TAG, "Error during dive profile item database insert!");
                }
            }
        } else {
            Log.e(TAG, "Error during dive profile database insert!");
        }

        return success;
    }

    public static boolean wasDiveProfileImported(SQLiteDatabase db, ProfileSummary profileSummary) {
        String[] columns = {
                SchemaContract.DiveProfileTable._ID,
                SchemaContract.DiveProfileTable.COLUMN_NAME_DIVE_DURATION,
                SchemaContract.DiveProfileTable.COLUMN_NAME_MAX_DEPTH,
                SchemaContract.DiveProfileTable.COLUMN_NAME_MIN_TEMPERATURE,
                SchemaContract.DiveProfileTable.COLUMN_NAME_OXYGEN_PERCENTAGE,
                SchemaContract.DiveProfileTable.COLUMN_NAME_DIVE_DATE_TIME
        };

        String selection = SchemaContract.DiveProfileTable.COLUMN_NAME_DIVE_DATE_TIME + " = ?";
        String[] selectionArgs = { profileSummary.getDiveDate() + " " + profileSummary.getDiveTime() };

        try (Cursor profileCursor = db.query(true, SchemaContract.DiveProfileTable.TABLE_NAME, columns, selection, selectionArgs, null, null, null, null)) {
            //Returns with false, if the Cursor object is empty
            return profileCursor.moveToFirst();
        }
    }

    public static Cursor getDiveProfiles(SQLiteDatabase db) {
        String[] columns = {
                SchemaContract.DiveProfileTable._ID,
                SchemaContract.DiveProfileTable.COLUMN_NAME_DIVE_DURATION,
                SchemaContract.DiveProfileTable.COLUMN_NAME_MAX_DEPTH,
                SchemaContract.DiveProfileTable.COLUMN_NAME_MIN_TEMPERATURE,
                SchemaContract.DiveProfileTable.COLUMN_NAME_OXYGEN_PERCENTAGE,
                SchemaContract.DiveProfileTable.COLUMN_NAME_DIVE_DATE_TIME
        };

        String orderBy = SchemaContract.DiveProfileTable.COLUMN_NAME_DIVE_DATE_TIME + " DESC";

        return db.query(true, SchemaContract.DiveProfileTable.TABLE_NAME, columns, null, null, null, null, orderBy, null);
    }

    public static DiveProfileEntry getDiveProfile(SQLiteDatabase db, Long diveProfileId) {
        String[] columns = {
                SchemaContract.DiveProfileTable._ID,
                SchemaContract.DiveProfileTable.COLUMN_NAME_DIVE_DURATION,
                SchemaContract.DiveProfileTable.COLUMN_NAME_MAX_DEPTH,
                SchemaContract.DiveProfileTable.COLUMN_NAME_MIN_TEMPERATURE,
                SchemaContract.DiveProfileTable.COLUMN_NAME_OXYGEN_PERCENTAGE,
                SchemaContract.DiveProfileTable.COLUMN_NAME_DIVE_DATE_TIME
        };

        String selection = SchemaContract.DiveProfileTable._ID + " = ?";
        String[] selectionArgs = { String.valueOf(diveProfileId) };

        DiveProfileEntry diveProfileEntry;
        try (Cursor diveProfileCursor =
                     db.query(true, SchemaContract.DiveProfileTable.TABLE_NAME, columns, selection, selectionArgs, null, null, null, null)) {
            diveProfileEntry = SchemaMapper.mapDiveProfileDetails(diveProfileCursor);
        }

        try (Cursor profileItemsCursor = getProfileItems(db, diveProfileId)) {
            diveProfileEntry = SchemaMapper.addProfileItemsToDiveProfileEntry(diveProfileEntry, profileItemsCursor);
        }
        return diveProfileEntry;
    }

    public static boolean deleteDiveProfile(SQLiteDatabase db, Long diveProfileId) {

        //Delete all Dive Profile Items
        String selection = SchemaContract.ProfileItemTable.COLUMN_NAME_DIVE_PROFILE_ID + " = ?";
        String[] selectionArgs = { String.valueOf(diveProfileId) };
        int rowsDeleted = db.delete(SchemaContract.ProfileItemTable.TABLE_NAME, selection, selectionArgs);
        Log.i(TAG, rowsDeleted + " rows deleted from " + SchemaContract.ProfileItemTable.TABLE_NAME);

        //Check if the Dive Profile Items were removed
        if (rowsDeleted > 0) {
            selection = SchemaContract.DiveProfileTable._ID + " = ?";
            rowsDeleted = db.delete(SchemaContract.DiveProfileTable.TABLE_NAME, selection, selectionArgs);
            Log.i(TAG, rowsDeleted + " rows deleted from " + SchemaContract.DiveProfileTable.TABLE_NAME);
        }

        //Deleting one profile, so in case of success, it has to be true
        if (rowsDeleted == 1) {
            return true;
        }
        return false;
    }

    static Cursor getProfileItems(SQLiteDatabase db, Long diveProfileId) {
        String[] columns = {
                SchemaContract.ProfileItemTable.COLUMN_NAME_DEPTH,
                SchemaContract.ProfileItemTable.COLUMN_NAME_PRESSURE,
                SchemaContract.ProfileItemTable.COLUMN_NAME_DURATION,
                SchemaContract.ProfileItemTable.COLUMN_NAME_TEMPERATURE
        };

        String selection = SchemaContract.ProfileItemTable.COLUMN_NAME_DIVE_PROFILE_ID + " = ?";
        String[] selectionArgs = { String.valueOf(diveProfileId) };

        String orderBy = SchemaContract.ProfileItemTable.COLUMN_NAME_DURATION + " ASC";

        return db.query(true, SchemaContract.ProfileItemTable.TABLE_NAME, columns, selection, selectionArgs, null, null, orderBy, null);
    }
}
