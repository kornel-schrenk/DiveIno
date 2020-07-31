package hu.diveino.droid;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ImageButton;
import android.widget.TextView;

import hu.diveino.droid.service.DiagnosticsUartGattCallback;
import hu.diveino.droid.service.SerialApiMessageType;

public class DiagnosticsBleActivity extends BaseBleActivity implements View.OnClickListener {

    private static final String TAG = "DiagnosticsActivity";

    private ImageButton temperatureSyncButton;
    private ImageButton pressureSyncButton;
    private ImageButton batteryChargeSyncButton;
    private ImageButton currentDateSyncButton;

    private TextView versionNumberTextView;
    private TextView temperatureTextView;
    private TextView pressureTextView;
    private TextView batteryChargeTextView;
    private TextView currentDateTextView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_diagnostics);

        this.versionNumberTextView = (TextView) this.findViewById(R.id.diagnostics_version_number);
        this.temperatureTextView = (TextView) this.findViewById(R.id.diagnostics_temperature);
        this.pressureTextView = (TextView) this.findViewById(R.id.diagnostics_pressure);
        this.batteryChargeTextView = (TextView) this.findViewById(R.id.diagnostics_battery);
        this.currentDateTextView = (TextView) this.findViewById(R.id.diagnostics_date);

        this.temperatureSyncButton = (ImageButton) this.findViewById(R.id.diagnostics_temperature_sync);
        this.temperatureSyncButton.setOnClickListener(this);

        this.pressureSyncButton = (ImageButton) this.findViewById(R.id.diagnostics_pressure_sync);
        this.pressureSyncButton.setOnClickListener(this);

        this.batteryChargeSyncButton = (ImageButton) this.findViewById(R.id.diagnostics_battery_charge_sync);
        this.batteryChargeSyncButton.setOnClickListener(this);

        this.currentDateSyncButton = (ImageButton) this.findViewById(R.id.diagnostics_date_sync);
        this.currentDateSyncButton.setOnClickListener(this);

        this.messageHandler = new DiagnosticsMessageHandler(Looper.getMainLooper());
        this.uartGattCallback = new DiagnosticsUartGattCallback(this.messageHandler);
    }

    @Override
    public void onStart() {
        super.onStart();
        this.callSerialApi(SerialApiMessageType.GENERAL_VERSION, "@VERSION#");
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.diagnostics, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        if (id == R.id.action_sync_all) {
            this.callSerialApi(SerialApiMessageType.GENERAL_VERSION, "@VERSION#");
            this.callSerialApi(SerialApiMessageType.DIAG_TEMP, "@DIAG TEMP#", 300);
            this.callSerialApi(SerialApiMessageType.DIAG_PRES, "@DIAG PRES#", 600);
            this.callSerialApi(SerialApiMessageType.DIAG_BAT, "@DIAG BAT#", 900);
            this.callSerialApi(SerialApiMessageType.DIAG_DATE, "@DIAG DATE#", 1200);
            return true;
        } else if (id == R.id.action_reset) {
            this.callSerialApi(SerialApiMessageType.GENERAL_RESET, "@RESET#");
            return true;
        } else if (id == R.id.action_clear) {
            this.callSerialApi(SerialApiMessageType.GENERAL_CLEAR_SURFACE, "@CLEAR#");
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onClick(View view) {
        if (this.temperatureSyncButton == view) {
            this.callSerialApi(SerialApiMessageType.DIAG_TEMP, "@DIAG TEMP#");
        } else if (this.pressureSyncButton == view) {
            this.callSerialApi(SerialApiMessageType.DIAG_PRES, "@DIAG PRES#");
        } else if (this.batteryChargeSyncButton == view) {
            this.callSerialApi(SerialApiMessageType.DIAG_BAT, "@DIAG BAT#");
        } else if (this.currentDateSyncButton == view) {
            this.callSerialApi(SerialApiMessageType.DIAG_DATE, "@DIAG DATE#");
        }
    }

    private class DiagnosticsMessageHandler extends Handler {

        DiagnosticsMessageHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case DiagnosticsUartGattCallback.MESSAGE_DIAG_TEMP_COMPLETED: {
                    Double temperature = (Double)msg.obj;
                    DiagnosticsBleActivity.this.temperatureTextView.setText(String.valueOf(temperature));
                } break;
                case DiagnosticsUartGattCallback.MESSAGE_DIAG_PRES_COMPLETED: {
                    Double pressure = (Double)msg.obj;
                    DiagnosticsBleActivity.this.pressureTextView.setText(String.valueOf(pressure));
                } break;
                case DiagnosticsUartGattCallback.MESSAGE_DIAG_BAT_COMPLETED: {
                    Double batteryCharge = (Double)msg.obj;
                    DiagnosticsBleActivity.this.batteryChargeTextView.setText(String.valueOf(batteryCharge));
                } break;
                case DiagnosticsUartGattCallback.MESSAGE_DIAG_DATE_COMPLETED: {
                    DiagnosticsBleActivity.this.currentDateTextView.setText(msg.obj.toString());
                } break;
                case DiagnosticsUartGattCallback.MESSAGE_GENERAL_VERSION_COMPLETED: {
                    DiagnosticsBleActivity.this.versionNumberTextView.setText(msg.obj.toString());
                } break;
            }
        }
    }
}
