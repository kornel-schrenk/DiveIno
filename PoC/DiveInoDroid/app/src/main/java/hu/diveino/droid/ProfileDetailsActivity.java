package hu.diveino.droid;

import android.app.LoaderManager;
import android.app.ProgressDialog;
import android.content.Intent;
import android.content.Loader;
import android.database.sqlite.SQLiteDatabase;
import android.graphics.Color;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.Description;
import com.github.mikephil.charting.components.Legend;
import com.github.mikephil.charting.components.LimitLine;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.formatter.DefaultFillFormatter;
import com.github.mikephil.charting.highlight.Highlight;
import com.github.mikephil.charting.listener.OnChartValueSelectedListener;
import com.nononsenseapps.filepicker.FilePickerActivity;
import com.nononsenseapps.filepicker.Utils;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import hu.diveino.droid.dao.DatabaseSchemaHelper;
import hu.diveino.droid.dao.SchemaQueries;
import hu.diveino.droid.loader.ProfileDetailsLoader;
import hu.diveino.droid.model.database.DiveProfileEntry;
import hu.diveino.droid.model.database.ProfileItemEntry;
import hu.diveino.droid.model.json.Profile;
import hu.diveino.droid.service.JsonParser;
import hu.diveino.droid.util.TypeConverter;
import hu.diveino.droid.util.picker.CustomizedFilePickerActivity;

public class ProfileDetailsActivity extends AppCompatActivity implements LoaderManager.LoaderCallbacks<DiveProfileEntry> {

    private static final String TAG = "ProfileDetails";

    private static final int PROFILE_DETAILS_LOADER = 3;

    private TextView durationValueTextView;
    private TextView oxygenPercentageValueTextView;
    private TextView minimumTemperatureValueTextView;
    private TextView maximumDepthValueTextView;
    private TextView diveDateValueTextView;
    private TextView diveTimeValueTextView;

    private TextView chartInfoDepthValueTextView;
    private TextView chartInfoDurationValueTextView;

    private LineChart profileChart;

    private ProgressDialog deletingDialog;

    private Long diveProfileId;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_profile_details);

        this.getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        this.diveProfileId = this.getIntent().getLongExtra("diveProfileId", -1L);

        this.durationValueTextView = (TextView) this.findViewById(R.id.profile_durationValueTextView);
        this.oxygenPercentageValueTextView = (TextView) this.findViewById(R.id.profile_oxygenPercentageValueTextView);
        this.minimumTemperatureValueTextView = (TextView) this.findViewById(R.id.profile_minimumTemperatureValueTextView);
        this.maximumDepthValueTextView = (TextView) this.findViewById(R.id.profile_maximumDepthValueTextView);
        this.diveDateValueTextView = (TextView) this.findViewById(R.id.profile_diveDateValueTextView);
        this.diveTimeValueTextView = (TextView) this.findViewById(R.id.profile_diveTimeValueTextView);

        this.chartInfoDepthValueTextView = (TextView) this.findViewById(R.id.profile_chart_info_depthValueTextView);
        this.chartInfoDurationValueTextView = (TextView) this.findViewById(R.id.profile_chart_info_durationValueTextView);

        this.profileChart = (LineChart) this.findViewById(R.id.profile_chart);
        this.setupLineChart(this.profileChart);

        this.deletingDialog = new ProgressDialog(this);
        this.deletingDialog.setIndeterminate(true);
        this.deletingDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
        this.deletingDialog.setMessage(getString(R.string.dialog_deleting));
        this.deletingDialog.setCancelable(true);
    }

    @Override
    protected void onStart() {
        super.onStart();
        this.getLoaderManager().initLoader(PROFILE_DETAILS_LOADER, null, this);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.profile_details, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == R.id.action_delete_profile) {
            this.deletingDialog.show();
            new DeleteProfileAsyncTask().execute(this.diveProfileId);
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public Loader<DiveProfileEntry> onCreateLoader(int id, Bundle args) {
        return new ProfileDetailsLoader(this, diveProfileId);
    }

    @Override
    public void onLoadFinished(Loader<DiveProfileEntry> loader, DiveProfileEntry data) {
        if (data != null) {
            this.durationValueTextView.setText(this.mapDiveDurationToScreen(data.getDiveDuration()));
            this.oxygenPercentageValueTextView.setText(TypeConverter.convertDoubleToText(data.getOxygenPercentage()));
            this.minimumTemperatureValueTextView.setText(TypeConverter.convertDoubleToText(data.getMinTemperature()));
            this.maximumDepthValueTextView.setText(TypeConverter.convertDoubleToText(data.getMaxDepth()));
            this.diveDateValueTextView.setText(data.getDiveDate());
            this.diveTimeValueTextView.setText(data.getDiveTime());

            Log.i(TAG, "Number of profile items: " + data.getProfileItems().size());
            this.drawDepthProfile(data.getProfileItems());
        }
    }

    @Override
    public void onLoaderReset(Loader<DiveProfileEntry> loader) {
    }

    private String mapDiveDurationToScreen(Integer diveDurationInSeconds) {
        Integer diveDurationMinutes = diveDurationInSeconds / 60;
        Integer diveDurationSeconds = diveDurationInSeconds % 60;
        return TypeConverter.convertIntegerToTwoCharacterText(diveDurationMinutes) + ":" + TypeConverter.convertIntegerToTwoCharacterText(diveDurationSeconds);
    }

    private LineChart setupLineChart(LineChart lineChart) {

        int axisTextColor = ContextCompat.getColor(this, R.color.axisTextColor);
        int axisLineColor = ContextCompat.getColor(this, R.color.axisLineColor);
        int legendColor = ContextCompat.getColor(this, R.color.legendColor);

        XAxis xAxis = lineChart.getXAxis();
        xAxis.setTextColor(axisTextColor);
        xAxis.setAxisLineColor(axisLineColor);
        xAxis.setDrawGridLines(false);
        xAxis.setDrawLabels(false);

        YAxis leftYAxis = lineChart.getAxisLeft();
        leftYAxis.setDrawGridLines(false);
        leftYAxis.setInverted(true);
        leftYAxis.setTextColor(axisTextColor);
        leftYAxis.setAxisLineColor(axisLineColor);

        YAxis rightYAxis = lineChart.getAxisRight();
        rightYAxis.setDrawGridLines(false);
        rightYAxis.setInverted(true);
        rightYAxis.setTextColor(axisTextColor);
        rightYAxis.setAxisLineColor(axisLineColor);

        Legend legend = lineChart.getLegend();
        legend.setEnabled(false);
        legend.setTextColor(legendColor);

        Description description = new Description();
        description.setText("");

        lineChart.setDrawGridBackground(false);
        lineChart.setDrawBorders(false);
        lineChart.setDescription(description);

        lineChart.invalidate();

        lineChart.setOnChartValueSelectedListener(
                new OnChartValueSelectedListener() {
                    @Override
                    public void onValueSelected(Entry e, Highlight h) {
                        if (e.getData() != null && e.getData() instanceof  ProfileItemEntry) {
                            ProfileItemEntry entry = (ProfileItemEntry)e.getData();
                            chartInfoDepthValueTextView.setText(String.valueOf(entry.getDepth()));

                            String minutes = TypeConverter.convertIntegerToText(entry.getDuration() / 60);
                            String seconds = TypeConverter.convertIntegerToTwoCharacterText(entry.getDuration() % 60);
                            chartInfoDurationValueTextView.setText(minutes + ":" + seconds);
                        }
                    }

                    @Override
                    public void onNothingSelected() {
                    }
                }
        );

        return lineChart;
    }

    private void drawDepthProfile(List<ProfileItemEntry> profileItemList) {

        int profileGraphColor = ContextCompat.getColor(this, R.color.profileGraphColor);
        int profileValueColor = ContextCompat.getColor(this, R.color.profileValueColor);
        int highLightColor = ContextCompat.getColor(this, R.color.highLightColor);

        int safetyStopLineColor = ContextCompat.getColor(this, R.color.safetyStopLineColor);
        int fortyMeterLineColor = ContextCompat.getColor(this, R.color.fortyMeterLineColor);
        int tenMeterLineColor = ContextCompat.getColor(this, R.color.tenMeterLineColor);

        List<Entry> profileDepthEntries = new ArrayList<>();
        List<String> profileDepthLabels = new ArrayList<>();

        int i = 0;
        for (ProfileItemEntry item : profileItemList) {
            float depth = item.getDepth().floatValue();
            profileDepthEntries.add(new Entry(i, depth, item));
            profileDepthLabels.add(String.valueOf(depth));
            i++;
        }

        LineDataSet profileDepthDataSet = new LineDataSet(profileDepthEntries, "Depth profile");
        Log.i(TAG, "Minimum: " + profileDepthDataSet.getYMin() + " Maximum: " + profileDepthDataSet.getYMax());

        profileDepthDataSet.setAxisDependency(YAxis.AxisDependency.LEFT);
        profileDepthDataSet.setDrawCircles(false);
        profileDepthDataSet.setCircleColor(profileGraphColor);
        profileDepthDataSet.setDrawValues(false);
        profileDepthDataSet.setColor(profileGraphColor);
        profileDepthDataSet.setValueTextColor(profileValueColor);
        profileDepthDataSet.setHighLightColor(highLightColor);

        YAxis leftYAxis = this.profileChart.getAxisLeft();
        //Do a clean-up before we add new limit lines
        leftYAxis.removeAllLimitLines();

        //Draw a line at 5 meters, which indicates the safety stop depth
        LimitLine ll = new LimitLine(5);
        ll.setLineColor(safetyStopLineColor);
        ll.setLineWidth(1f);
        leftYAxis.addLimitLine(ll);

        //Draw a line at 40 meters, which is the limit of the recreational scuba diving
        if (profileDepthDataSet.getYMax() >= 40) {
            ll = new LimitLine(40);
            ll.setLineColor(fortyMeterLineColor);
            ll.setLineWidth(1f);
            leftYAxis.addLimitLine(ll);
        }

        //Draw a line in every 10 meters depth
        float j = 10;
        while (j <= profileDepthDataSet.getYMax()) {
            ll = new LimitLine(j);
            ll.setLineColor(tenMeterLineColor);
            ll.setLineWidth(1f);
            leftYAxis.addLimitLine(ll);
            j = j + 10;
            if (j == 40) {
                //Skip the 40 meter line, because it will be drawn in different color
                j = 50;
            }
            Log.i(TAG, "Limit line was drawn at " + j);
        }

        LineData lineChartData = new LineData(profileDepthDataSet);
        this.profileChart.setData(lineChartData);
        this.profileChart.notifyDataSetChanged();
        this.profileChart.invalidate();
    }

    private class DeleteProfileAsyncTask extends AsyncTask<Long, Integer, Boolean> {

        @Override
        protected Boolean doInBackground(Long... params) {
            DatabaseSchemaHelper databaseSchemaHelper = new DatabaseSchemaHelper(ProfileDetailsActivity.this);
            SQLiteDatabase database = databaseSchemaHelper.getWritableDatabase();

            Boolean success = Boolean.FALSE;
            for (Long diveProfileId : params) {
                success = SchemaQueries.deleteDiveProfile(database, diveProfileId);
            }

            database.close();
            databaseSchemaHelper.close();

            return success;
        }

        @Override
        protected void onProgressUpdate(Integer... progress) {
            //TODO Show profile delete progress on a ProgressBar
        }

        @Override
        protected void onPostExecute(Boolean success) {
            if (success) {
                Intent intent = new Intent(ProfileDetailsActivity.this, LogbookActivity.class);
                intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                startActivity(intent);
            } else {
                Toast.makeText(ProfileDetailsActivity.this, R.string.notification_error_profile_delete, Toast.LENGTH_SHORT).show();
            }
            deletingDialog.hide();
        }
    }
}
