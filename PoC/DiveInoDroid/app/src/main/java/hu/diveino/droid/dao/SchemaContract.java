package hu.diveino.droid.dao;

import android.provider.BaseColumns;

public final class SchemaContract {

    private SchemaContract() {}

    public static class DiveProfileTable implements BaseColumns {
        public static final String TABLE_NAME = "DIVE_PROFILE";
        public static final String COLUMN_NAME_DIVE_DURATION = "DIVE_DURATION";
        public static final String COLUMN_NAME_MAX_DEPTH = "MAX_DEPTH";
        public static final String COLUMN_NAME_MIN_TEMPERATURE = "MIN_TEMPERATURE";
        public static final String COLUMN_NAME_OXYGEN_PERCENTAGE = "OXYGEN_PERCENTAGE";
        public static final String COLUMN_NAME_DIVE_DATE_TIME = "DIVE_DATE_TIME";
    }

    public static class ProfileItemTable implements BaseColumns {
        public static final String TABLE_NAME = "PROFILE_ITEM";
        public static final String COLUMN_NAME_DIVE_PROFILE_ID = "DIVE_PROFILE_ID";
        public static final String COLUMN_NAME_DEPTH = "DEPTH";
        public static final String COLUMN_NAME_PRESSURE = "PRESSURE";
        public static final String COLUMN_NAME_DURATION = "DURATION";
        public static final String COLUMN_NAME_TEMPERATURE = "TEMPERATURE";
    }

}
