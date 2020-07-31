package hu.diveino.droid.dao;

import android.database.Cursor;
import android.util.Log;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;

import hu.diveino.droid.model.database.DiveProfileEntry;
import hu.diveino.droid.model.database.LogbookEntry;
import hu.diveino.droid.model.database.ProfileItemEntry;

public final class SchemaMapper {

    private static final String TAG = "SchemaMapper";

    private SchemaMapper() {}

    static LogbookEntry mapDiveProfilesToLogbookEntry(Cursor diveProfilesCursor) {

        Integer numberOfDives = 0;
        Long totalDiveSeconds = 0L;
        Double maxDepth = 0.0;
        String lastDiveDateTime = "";    //yyyy-MM-dd HH:mm

        SimpleDateFormat diveDateTimeFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm");

        Calendar calendar = GregorianCalendar.getInstance();
        calendar.set(1900,1,1);
        Date latestDiveDate = calendar.getTime();

        while (diveProfilesCursor.moveToNext()) {
            totalDiveSeconds = totalDiveSeconds + diveProfilesCursor.getInt(diveProfilesCursor.getColumnIndex(SchemaContract.DiveProfileTable.COLUMN_NAME_DIVE_DURATION));

            Double profileDepth = diveProfilesCursor.getDouble(diveProfilesCursor.getColumnIndex(SchemaContract.DiveProfileTable.COLUMN_NAME_MAX_DEPTH));
            if (profileDepth > maxDepth) {
                maxDepth = profileDepth;
            }

            String diveDateTime = diveProfilesCursor.getString(diveProfilesCursor.getColumnIndex(SchemaContract.DiveProfileTable.COLUMN_NAME_DIVE_DATE_TIME ));
            try {
                Date diveDate = diveDateTimeFormat.parse(diveDateTime);
                if (diveDate.after(latestDiveDate)) {
                    latestDiveDate = diveDate;
                    lastDiveDateTime = diveDateTime;
                }
            } catch (ParseException e) {
                Log.w(TAG, "Wrong date format: " + diveDateTime);
            }

            numberOfDives++;
        }

        //Compute these values based on the total dive seconds
        Integer loggedDiveHours = (int) (totalDiveSeconds / 3600);
        Integer loggedDiveMinutes = (int) (totalDiveSeconds % 3600 / 60);

        return new LogbookEntry(numberOfDives, loggedDiveHours, loggedDiveMinutes, maxDepth, lastDiveDateTime);
    }

    static DiveProfileEntry mapDiveProfileDetails(Cursor diveProfileCursor) {
        if (diveProfileCursor.moveToFirst()) {
            Long id = diveProfileCursor.getLong(diveProfileCursor.getColumnIndex(SchemaContract.DiveProfileTable._ID));
            Integer diveDuration = diveProfileCursor.getInt(diveProfileCursor.getColumnIndex(SchemaContract.DiveProfileTable.COLUMN_NAME_DIVE_DURATION));
            Double maxDepth = diveProfileCursor.getDouble(diveProfileCursor.getColumnIndex(SchemaContract.DiveProfileTable.COLUMN_NAME_MAX_DEPTH));
            Double minTemperature = diveProfileCursor.getDouble(diveProfileCursor.getColumnIndex(SchemaContract.DiveProfileTable.COLUMN_NAME_MIN_TEMPERATURE));
            Double oxygenPercentage = diveProfileCursor.getDouble(diveProfileCursor.getColumnIndex(SchemaContract.DiveProfileTable.COLUMN_NAME_OXYGEN_PERCENTAGE));
            String diveDateTime = diveProfileCursor.getString(diveProfileCursor.getColumnIndex(SchemaContract.DiveProfileTable.COLUMN_NAME_DIVE_DATE_TIME));
            return new DiveProfileEntry(id, diveDuration, maxDepth, minTemperature, oxygenPercentage, diveDateTime);
        }
        return null;
    }

    static DiveProfileEntry addProfileItemsToDiveProfileEntry(DiveProfileEntry diveProfileEntry, Cursor profileItemsCursor) {
        if (diveProfileEntry != null) {
            while (profileItemsCursor.moveToNext()) {
                Double depth = profileItemsCursor.getDouble(profileItemsCursor.getColumnIndex(SchemaContract.ProfileItemTable.COLUMN_NAME_DEPTH));
                Double pressure = profileItemsCursor.getDouble(profileItemsCursor.getColumnIndex(SchemaContract.ProfileItemTable.COLUMN_NAME_PRESSURE));
                Integer duration = profileItemsCursor.getInt(profileItemsCursor.getColumnIndex(SchemaContract.ProfileItemTable.COLUMN_NAME_DURATION));
                Double temperature = profileItemsCursor.getDouble(profileItemsCursor.getColumnIndex(SchemaContract.ProfileItemTable.COLUMN_NAME_TEMPERATURE));

                diveProfileEntry.getProfileItems().add(new ProfileItemEntry(depth, pressure, duration, temperature));
            }
        }
        return diveProfileEntry;
    }
}
