package hu.diveino.droid.loader;

import android.app.ProgressDialog;
import android.content.AsyncTaskLoader;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

import hu.diveino.droid.dao.DatabaseSchemaHelper;
import hu.diveino.droid.dao.SchemaQueries;
import hu.diveino.droid.model.database.DiveProfileEntry;

public class ProfileDetailsLoader extends AsyncTaskLoader<DiveProfileEntry> {

    // We hold a reference to the Loader’s data here.
    private DiveProfileEntry mData;

    private SQLiteDatabase database;
    private DatabaseSchemaHelper databaseSchemaHelper;

    private Long diveProfileId;

    public ProfileDetailsLoader(Context ctx, Long diveProfileId) {
        super(ctx);
        this.databaseSchemaHelper = new DatabaseSchemaHelper(ctx);
        this.diveProfileId = diveProfileId;
    }

    @Override
    public DiveProfileEntry loadInBackground() {
        this.database = this.databaseSchemaHelper.getReadableDatabase();
        return SchemaQueries.getDiveProfile(this.database, this.diveProfileId);
    }

    @Override
    public void deliverResult(DiveProfileEntry data) {
        if (isReset()) {
            releaseResources(data);
            return;
        }

        DiveProfileEntry oldData = mData;
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
    public void onCanceled(DiveProfileEntry data) {
        super.onCanceled(data);

        releaseResources(data);
    }

    private void releaseResources(DiveProfileEntry data) {
        if (this.database != null && this.database.isOpen()){
            this.database.close();
        }
        if (this.databaseSchemaHelper != null) {
            this.databaseSchemaHelper.close();
        }
    }
}
