package hu.diveino.droid.dao;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.util.Log;

public class DatabaseSchemaHelper extends SQLiteOpenHelper {

    private static final String TAG = "DatabaseSchemaHelper";

    public static final int DATABASE_VERSION = 1;
    public static final String DATABASE_NAME = "DiveIno.db";

    public DatabaseSchemaHelper(Context context) {
        super(context, DATABASE_NAME, null, DATABASE_VERSION);
    }
    public void onCreate(SQLiteDatabase db) {
        db.execSQL(SchemaQueries.SQL_CREATE_DIVE_PROFILE);
        db.execSQL(SchemaQueries.SQL_CREATE_PROFILE_ITEM);
        Log.i(TAG, "DiveIno database schema was created.");
    }
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        //TODO - Execute the upgrade script
        Log.i(TAG, "DiveIno database schema was updated.");
    }

    public void onDowngrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        //TODO - Execute the downgrade script
        Log.i(TAG, "DiveIno database schema was downgraded.");
    }
}
