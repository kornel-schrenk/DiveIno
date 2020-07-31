package hu.diveino.droid;

import android.app.Activity;
import android.app.LoaderManager;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.content.Loader;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.CursorAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.nononsenseapps.filepicker.FilePickerActivity;
import com.nononsenseapps.filepicker.Utils;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

import hu.diveino.droid.dao.DatabaseSchemaHelper;
import hu.diveino.droid.dao.SchemaContract;
import hu.diveino.droid.dao.SchemaQueries;
import hu.diveino.droid.loader.DiveProfilesLoader;
import hu.diveino.droid.loader.LogbookSummaryLoader;
import hu.diveino.droid.model.database.LogbookEntry;
import hu.diveino.droid.model.json.Profile;
import hu.diveino.droid.service.JsonParser;
import hu.diveino.droid.util.picker.CustomizedFilePickerActivity;

public class LogbookActivity extends AppCompatActivity
        implements AdapterView.OnItemClickListener, LoaderManager.LoaderCallbacks<Object> {

    private static final String TAG = "LogbookActivity";

    private static final int FILE_CODE = 999;

    private static final int LOGBOOK_LOADER = 1;
    private static final int DIVE_PROFILES_LOADER = 2;

    private TextView totalDivesValueTextView;
    private TextView totalDurationValueTextView;
    private TextView maximumDepthValueTextView;
    private TextView lastDiveDateValueTextView;
    private TextView lastDiveTimeValueTextView;

    private ListView divesListView;
    private DiveProfileCursorAdapter listAdapter;

    private ProgressDialog loadingDialog;

    private LogbookEntry logbookEntry;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_logbook);

        this.getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        this.totalDivesValueTextView = (TextView) this.findViewById(R.id.logbook_totalDivesValueTextView);
        this.totalDurationValueTextView = (TextView) this.findViewById(R.id.logbook_totalDurationValueTextView);
        this.maximumDepthValueTextView = (TextView) this.findViewById(R.id.logbook_maximumDepthValueTextView);
        this.lastDiveDateValueTextView = (TextView) this.findViewById(R.id.logbook_lastDiveDateValueTextView);
        this.lastDiveTimeValueTextView = (TextView) this.findViewById(R.id.logbook_lastDiveTimeValueTextView);

        this.divesListView = (ListView)findViewById(R.id.divesListView);
        this.divesListView.setOnItemClickListener(this);

        String[] fromColumns = { SchemaContract.DiveProfileTable.COLUMN_NAME_DIVE_DATE_TIME };
        int[] toViews = { R.id.list_diveDateTime };

        // Create an empty adapter we will use to display the loaded data.
        // We pass null for the cursor, then update it in onLoadFinished()
        this.listAdapter = new DiveProfileCursorAdapter(this, null);
        this.divesListView.setAdapter(this.listAdapter);

        this.loadingDialog = new ProgressDialog(this);
        this.loadingDialog.setIndeterminate(true);
        this.loadingDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
        this.loadingDialog.setMessage(getString(R.string.dialog_loading));
        this.loadingDialog.setCancelable(true);
    }

    @Override
    protected void onStart() {
        super.onStart();
        this.getLoaderManager().initLoader(LOGBOOK_LOADER, null, this);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.logbook, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == R.id.action_open_file) {

            // This always works
            Intent i = new Intent(this, CustomizedFilePickerActivity.class);
            // This works if you defined the intent filter
            // Intent i = new Intent(Intent.ACTION_GET_CONTENT);

            // Set these depending on your use case. These are the defaults.
            i.putExtra(FilePickerActivity.EXTRA_ALLOW_MULTIPLE, true);
            i.putExtra(FilePickerActivity.EXTRA_ALLOW_CREATE_DIR, false);
            i.putExtra(FilePickerActivity.EXTRA_MODE, FilePickerActivity.MODE_FILE);

            // Configure initial directory by specifying a String.
            // You could specify a String like "/storage/emulated/0/", but that can
            // dangerous. Always use Android's API calls to get paths to the SD-card or
            // internal memory.
            i.putExtra(FilePickerActivity.EXTRA_START_PATH, Environment.getExternalStorageDirectory().getPath());

            startActivityForResult(i, FILE_CODE);
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == FILE_CODE && resultCode == Activity.RESULT_OK) {
            this.loadingDialog.show();
            // Use the provided utility method to parse the result
            List<Uri> files = Utils.getSelectedFilesFromResult(data);
            new ImportProfileAsyncTask().execute(files);
        }
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        Log.i(TAG, "Dive profile was selected: " + position + ", " + id);

        Intent intent = new Intent(this, ProfileDetailsActivity.class);
        intent.putExtra("diveProfileId", id);
        startActivity(intent);
    }

    @Override
    public Loader<Object> onCreateLoader(int id, Bundle args) {
        if (id == DIVE_PROFILES_LOADER && this.logbookEntry != null) {
            return new DiveProfilesLoader(this);
        }
        return new LogbookSummaryLoader(this);
    }

    @Override
    public void onLoadFinished(Loader<Object> loader, Object data) {
        if (loader.getId() == LOGBOOK_LOADER) {
            if (data != null && data instanceof LogbookEntry) {
                this.logbookEntry = (LogbookEntry) data;

                //Update the screen with the loaded data
                this.totalDivesValueTextView.setText(logbookEntry.getNumberOfDivesText());
                this.maximumDepthValueTextView.setText(logbookEntry.getMaxDepthText());
                this.totalDurationValueTextView.setText(logbookEntry.getLoggedDiveHoursText() + ":" + logbookEntry.getLoggedDiveMinutesText());

                if (!"".equals(logbookEntry.getLastDiveDateTime()) && logbookEntry.getLastDiveDateTime() != null) {
                    this.lastDiveDateValueTextView.setText(logbookEntry.getLastDiveDate());
                    this.lastDiveTimeValueTextView.setText(logbookEntry.getLastDiveTime());
                } else {
                    this.lastDiveDateValueTextView.setText(R.string.not_available);
                    this.lastDiveTimeValueTextView.setText("");
                }

                //Load dive profiles for the current logbook
                this.getLoaderManager().initLoader(DIVE_PROFILES_LOADER, null, this);
            }
        } else if (loader.getId() == DIVE_PROFILES_LOADER) {
            if (data != null && data instanceof Cursor) {
                this.listAdapter.swapCursor((Cursor) data);
            }
        }
    }

    @Override
    public void onLoaderReset(Loader<Object> loader) {
        if (loader instanceof DiveProfilesLoader) {
            this.listAdapter.swapCursor(null);
        }
    }

    private class ImportProfileAsyncTask extends AsyncTask<List<Uri>, Integer, List<String>> {

        private int ERROR_ALREADY_IMPORTED = -2;

        @Override
        protected List<String> doInBackground(List<Uri>... params) {
            List<String> importedFileNames = new ArrayList<>();

            DatabaseSchemaHelper databaseSchemaHelper = new DatabaseSchemaHelper(LogbookActivity.this);
            SQLiteDatabase database = databaseSchemaHelper.getWritableDatabase();

            for (List<Uri> files : params) {
                for (Uri uri : files) {
                    File file = Utils.getFileForUri(uri);
                    Log.i(TAG, "Log file to be imported: " + file.getName());

                    String jsonContent = this.readFileContent(file);
                    if (!"".equals(jsonContent)) {
                        Profile profile = JsonParser.parseProfile(jsonContent);
                        if (!SchemaQueries.wasDiveProfileImported(database, profile.getProfileSummary())) {
                            if (SchemaQueries.addDiveProfile(database, profile)) {
                                importedFileNames.add(file.getName());
                            }
                        } else {
                            this.publishProgress(ERROR_ALREADY_IMPORTED);
                            Log.w(TAG, file.getName() + " was previously imported.");
                        }
                    }
                }
            }

            database.close();
            databaseSchemaHelper.close();

            return importedFileNames;
        }

        @Override
        protected void onProgressUpdate(Integer... progress) {
            if (ERROR_ALREADY_IMPORTED == progress[0]) {
                Toast.makeText(LogbookActivity.this, R.string.notification_already_imported_profile, Toast.LENGTH_SHORT).show();
            }
            //TODO Show file import progress on a ProgressBar
        }

        @Override
        protected void onPostExecute(List<String> importedFileNames) {
            for (String fileName : importedFileNames) {
                Log.i(TAG, fileName + " was imported");
            }

            if (!importedFileNames.isEmpty()) {
                getLoaderManager().restartLoader(LOGBOOK_LOADER, null, LogbookActivity.this);
                getLoaderManager().restartLoader(DIVE_PROFILES_LOADER, null, LogbookActivity.this);
            }
            loadingDialog.hide();
        }

        private String readFileContent(File file) {
            StringBuilder stringBuilder = new StringBuilder("");
            try {
                FileInputStream inputStream = new FileInputStream(file);
                BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
                String receivedString = bufferedReader.readLine();
                while (receivedString != null) {
                    stringBuilder = stringBuilder.append(receivedString);
                    receivedString = bufferedReader.readLine();
                }
                inputStream.close();
            } catch (FileNotFoundException e) {
                Log.e(TAG, "File not found: " + file.getName(), e);
            } catch (IOException e) {
                Log.e(TAG, e.getMessage(), e);
            }
            return stringBuilder.toString();
        }
    }

    private class DiveProfileCursorAdapter extends CursorAdapter {

        DiveProfileCursorAdapter(Context context, Cursor cursor) {
            super(context, cursor, 0);
        }

        @Override
        public View newView(Context context, Cursor cursor, ViewGroup parent) {
            return LayoutInflater.from(context).inflate(R.layout.listitem_profile, parent, false);
        }

        @Override
        public void bindView(View view, Context context, Cursor cursor) {
            // Find fields to populate in inflated template
            TextView diveNumberTextView = (TextView) view.findViewById(R.id.list_diveNumber);
            TextView diveDateTimeTextView = (TextView) view.findViewById(R.id.list_diveDateTime);

            // Extract properties from cursor
            String diveDateTime = cursor.getString(cursor.getColumnIndexOrThrow(SchemaContract.DiveProfileTable.COLUMN_NAME_DIVE_DATE_TIME));

            // Populate fields with extracted properties
            diveNumberTextView.setText(String.valueOf(cursor.getPosition()+1));
            diveDateTimeTextView.setText(diveDateTime);
        }
    }
}
