package hu.diveino.droid.loader;

import android.content.AsyncTaskLoader;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

import hu.diveino.droid.dao.DatabaseSchemaHelper;
import hu.diveino.droid.dao.SchemaQueries;

public class DiveProfilesLoader<C> extends AsyncTaskLoader<Cursor> {

    // We hold a reference to the Loader’s data here.
    private Cursor mData;

    private SQLiteDatabase database;
    private DatabaseSchemaHelper databaseSchemaHelper;

    public DiveProfilesLoader(Context ctx) {
        super(ctx);
        this.databaseSchemaHelper = new DatabaseSchemaHelper(ctx);
    }

    @Override
    public Cursor loadInBackground() {
        this.database = this.databaseSchemaHelper.getReadableDatabase();
        return SchemaQueries.getDiveProfiles(this.database);
    }

    @Override
    public void deliverResult(Cursor data) {
        if (isReset()) {
            releaseResources(data);
            return;
        }

        Cursor oldData = mData;
        mData = data;

        if (isStarted()) {
            super.deliverResult(data);
        }

        if (oldData != null && oldData != data) {
            releaseResources(oldData);
        }
    }


    @Override
    protected void onStartLoading() {
        if (mData != null) {
            deliverResult(mData);
        }

        if (takeContentChanged() || mData == null) {
            forceLoad();
        }
    }

    @Override
    protected void onStopLoading() {
        cancelLoad();
    }

    @Override
    protected void onReset() {
        onStopLoading();

        if (mData != null) {
            releaseResources(mData);
            mData = null;
        }
    }

    @Override
    public void onCanceled(Cursor data) {
        super.onCanceled(data);

        releaseResources(data);
    }

    private void releaseResources(Cursor data) {
        data.close();

        if (this.database != null && this.database.isOpen()){
            this.database.close();
        }
        if (this.databaseSchemaHelper != null) {
            this.databaseSchemaHelper.close();
        }
    }
}
