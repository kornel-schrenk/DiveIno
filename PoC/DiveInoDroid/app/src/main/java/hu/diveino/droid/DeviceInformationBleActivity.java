package hu.diveino.droid;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.TextView;

import hu.diveino.droid.service.DeviceInformationUartGattCallback;
import hu.diveino.droid.service.SerialApiMessageType;
import hu.diveino.droid.model.json.Logbook;
import hu.diveino.droid.model.json.Profile;

public class DeviceInformationBleActivity extends BaseBleActivity
        implements SeekBar.OnSeekBarChangeListener, View.OnClickListener {

    private static final String TAG = "InformationActivity";

    private TextView totalDivesValueTextView;
    private TextView storedProfilesValueTextView;
    private TextView maximumDepthValueTextView;
    private TextView allDurationValueTextView;
    private TextView lastDiveDateValueTextView;
    private TextView lastDiveTimeValueTextView;

    private SeekBar downloadImportSeekBar;
    private EditText downloadImportEditText;
    private Button downloadImportButton;

    private int importSeekBarOffset = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_device_information);

        this.totalDivesValueTextView = (TextView) this.findViewById(R.id.information_totalDivesValueTextView);
        this.storedProfilesValueTextView = (TextView) this.findViewById(R.id.information_storedProfilesValueTextView);
        this.maximumDepthValueTextView = (TextView) this.findViewById(R.id.information_maximumDepthValueTextView);
        this.allDurationValueTextView = (TextView) this.findViewById(R.id.information_allDurationValueTextView);
        this.lastDiveDateValueTextView = (TextView) this.findViewById(R.id.information_lastDiveDateValueTextView);
        this.lastDiveTimeValueTextView = (TextView) this.findViewById(R.id.information_lastDiveTimeValueTextView);

        this.downloadImportSeekBar = (SeekBar) this.findViewById(R.id.download_import_seekBar);
        this.downloadImportSeekBar.setOnSeekBarChangeListener(this);

        this.downloadImportEditText = (EditText) this.findViewById(R.id.download_import_edit);
        this.downloadImportButton = (Button) this.findViewById(R.id.download_import_button);
        this.downloadImportButton.setOnClickListener(this);
        this.downloadImportButton.setEnabled(false);

        this.messageHandler = new DeviceInformationBleActivity.DeviceInformationMessageHandler(Looper.getMainLooper());
        this.uartGattCallback = new DeviceInformationUartGattCallback(this.messageHandler);
    }

    @Override
    public void onStart() {
        super.onStart();
        this.callSerialApi(SerialApiMessageType.LOGBOOK_INFO, "@LOGBOOK#");
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        if (seekBar == this.downloadImportSeekBar) {
            this.downloadImportEditText.setText(String.valueOf(importSeekBarOffset + progress));
        }
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.device_information, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        if (id == R.id.action_sync_all) {
            this.callSerialApi(SerialApiMessageType.LOGBOOK_INFO, "@LOGBOOK#");
            return true;
        } else if (item.getItemId() == R.id.action_import_all) {
            //TODO Implement import all action
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onClick(View view) {
        if (this.downloadImportButton == view) {
            int progress = downloadImportSeekBar.getProgress();
            Log.i(TAG, "Import profile index: " + progress+1);
            this.callSerialApi(SerialApiMessageType.LOGBOOK_PROFILE_INFO, "@PROFILE " + String.valueOf(progress+1) + "#");
        }
    }

    private class DeviceInformationMessageHandler extends Handler {

        DeviceInformationMessageHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case DeviceInformationUartGattCallback.MESSAGE_LOGBOOK_INFO_COMPLETED: {
                    Logbook logbook = (Logbook)msg.obj;
                    int numberOfStoredProfiles = logbook.getNumberOfStoredProfiles();

                    totalDivesValueTextView.setText(String.valueOf(logbook.getNumberOfDives()));
                    storedProfilesValueTextView.setText(String.valueOf(numberOfStoredProfiles));
                    maximumDepthValueTextView.setText(String.valueOf(logbook.getMaxDepth()));
                    allDurationValueTextView.setText(logbook.getLoggedDiveHours() + ":" + logbook.getLoggedDiveMinutes());
                    lastDiveDateValueTextView.setText(logbook.getLastDiveDate());
                    lastDiveTimeValueTextView.setText(logbook.getLastDiveTime());

                    if (numberOfStoredProfiles > 0) {
                        downloadImportSeekBar.setMax(numberOfStoredProfiles-1);
                    }
                    downloadImportEditText.setText("1");
                    downloadImportButton.setEnabled(true);
                } break;
                case DeviceInformationUartGattCallback.MESSAGE_PROFILE_INFO_COMPLETED: {
                    Profile profile = (Profile)msg.obj;
                    Log.i(TAG, "Profile items count: " + profile.getProfileItems().size());
                    //TODO Import the profile into the logbook
                } break;
            }
        }
    }
}
