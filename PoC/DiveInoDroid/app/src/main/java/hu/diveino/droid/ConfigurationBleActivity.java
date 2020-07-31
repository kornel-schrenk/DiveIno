package hu.diveino.droid;

import android.app.DatePickerDialog;
import android.app.TimePickerDialog;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.DatePicker;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.TimePicker;

import java.text.DecimalFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Locale;

import hu.diveino.droid.service.ConfigurationUartGattCallback;
import hu.diveino.droid.service.SerialApiMessageType;
import hu.diveino.droid.model.json.Settings;

public class ConfigurationBleActivity extends BaseBleActivity
        implements SeekBar.OnSeekBarChangeListener, Switch.OnCheckedChangeListener, EditText.OnClickListener{

    private static final String TAG = "ConfigurationActivity";

    private static DecimalFormat oxygenPercentageFormat = new DecimalFormat(".#");
    private static SimpleDateFormat currentDateFormat = new SimpleDateFormat("yyyy-MM-dd", Locale.US);
    private static SimpleDateFormat currentTimeFormat = new SimpleDateFormat("HH:mm", Locale.US);

    private static SimpleDateFormat messageDateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm", Locale.US);
    private static SimpleDateFormat receivedMessageDateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.US);

    private SeekBar pressureSeekBar;
    private EditText pressureEditText;

    private SeekBar oxygenSeekBar;
    private EditText oxygenEditText;

    private Switch soundSwitch;
    private TextView soundValueTextView;

    private Switch unitsSwitch;
    private TextView unitsValueTextView;

    private EditText dateEditText;
    private DatePickerDialog datePickerDialog;

    private EditText timeEditText;
    private TimePickerDialog timePickerDialog;

    private ImageButton nowButton;

    private Calendar calendar = Calendar.getInstance();

    private int pressureSeekBarOffset = 980; //1050
    private int oxygenSeekBarOffset = 210; //400 - multiplied by 10

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_configuration);

        this.pressureEditText = (EditText) this.findViewById(R.id.config_pressure_edit);
        this.pressureEditText.setText(String.valueOf(1013));

        this.pressureSeekBar = (SeekBar) this.findViewById(R.id.config_pressure_seekBar);
        this.pressureSeekBar.setProgress(1013-pressureSeekBarOffset);
        this.pressureSeekBar.setOnSeekBarChangeListener(this);

        this.oxygenEditText = (EditText) this.findViewById(R.id.config_oxygen_edit);
        this.oxygenEditText.setText(oxygenPercentageFormat.format(21.0));

        this.oxygenSeekBar = (SeekBar) this.findViewById(R.id.config_oxygen_seekBar);
        this.oxygenSeekBar.setOnSeekBarChangeListener(this);

        this.soundSwitch = (Switch) this.findViewById(R.id.config_sound_switch);
        this.soundSwitch.setOnCheckedChangeListener(this);
        this.soundValueTextView = (TextView) this.findViewById(R.id.config_sound_switch_value);

        this.unitsSwitch = (Switch) this.findViewById(R.id.config_units_switch);
        this.unitsSwitch.setOnCheckedChangeListener(this);
        this.unitsValueTextView = (TextView) this.findViewById(R.id.config_units_switch_value);

        this.dateEditText = (EditText) this.findViewById(R.id.config_date_editText);
        this.dateEditText.setOnClickListener(this);
        this.dateEditText.setText(currentDateFormat.format(calendar.getTime()));

        this.nowButton = (ImageButton) this.findViewById(R.id.config_time_now_button);
        this.nowButton.setOnClickListener(this);

        DatePickerDialog.OnDateSetListener datePickerListener = new DatePickerDialog.OnDateSetListener() {
            @Override
            public void onDateSet(DatePicker view, int year, int monthOfYear, int dayOfMonth) {
                calendar.set(Calendar.YEAR, year);
                calendar.set(Calendar.MONTH, monthOfYear);
                calendar.set(Calendar.DAY_OF_MONTH, dayOfMonth);
                dateEditText.setText(currentDateFormat.format(calendar.getTime()));

                String messageValue = messageDateFormat.format(calendar.getTime());
                callSerialApi(SerialApiMessageType.SETTINGS_DATETIME, "@DATETIME " + messageValue + "#");
            }
        };
        this.datePickerDialog = new DatePickerDialog(this, datePickerListener, calendar.get(Calendar.YEAR),
                calendar.get(Calendar.MONTH), calendar.get(Calendar.DAY_OF_MONTH));

        this.timeEditText = (EditText) this.findViewById(R.id.config_time_editText);
        this.timeEditText.setOnClickListener(this);
        this.timeEditText.setText(currentTimeFormat.format(calendar.getTime()));

        TimePickerDialog.OnTimeSetListener timePickerListener = new TimePickerDialog.OnTimeSetListener() {
            public void onTimeSet(TimePicker view, int hourOfDay, int minute) {
                calendar.set(Calendar.HOUR_OF_DAY, hourOfDay);
                calendar.set(Calendar.MINUTE, minute);
                timeEditText.setText(currentTimeFormat.format(calendar.getTime()));

                String messageValue = messageDateFormat.format(calendar.getTime());
                callSerialApi(SerialApiMessageType.SETTINGS_DATETIME, "@DATETIME " + messageValue + "#");
            }
        };
        this.timePickerDialog = new TimePickerDialog(this, timePickerListener, calendar.get(Calendar.HOUR_OF_DAY),
                calendar.get(Calendar.MINUTE), true);

        this.messageHandler = new ConfigurationBleActivity.ConfigurationMessageHandler(Looper.getMainLooper());
        this.uartGattCallback = new ConfigurationUartGattCallback(this.messageHandler);
    }

    @Override
    public void onStart() {
        super.onStart();
        this.callSerialApi(SerialApiMessageType.SETTINGS_INFO, "@SETTINGS#");
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        if (seekBar == this.pressureSeekBar) {
            this.pressureEditText.setText(String.valueOf(pressureSeekBarOffset + progress));
        } else if (seekBar == this.oxygenSeekBar) {
            double value = oxygenSeekBarOffset + progress;
            this.oxygenEditText.setText(oxygenPercentageFormat.format(value/10));
        }
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        int progress = seekBar.getProgress();
        if (seekBar == this.pressureSeekBar) {
            Log.i(TAG, "Selected pressure value: " + String.valueOf(pressureSeekBarOffset + progress));
            this.callSerialApi(SerialApiMessageType.SETTINGS_PRESSURE, "@PRESSURE "+ String.valueOf(pressureSeekBarOffset + progress) + "#");
        } else if (seekBar == this.oxygenSeekBar) {
            double value = oxygenSeekBarOffset + progress;
            Log.i(TAG, "Selected oxygen percentage value: " + String.valueOf(value/1000));
            this.callSerialApi(SerialApiMessageType.SETTINGS_OXYGEN, "@OXYGEN " + String.valueOf(value/1000) + "#");
        }
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        if (buttonView == this.soundSwitch) {
            if (isChecked) {
                this.callSerialApi(SerialApiMessageType.SETTINGS_SOUND, "@SOUND ON#");
            } else {
                this.callSerialApi(SerialApiMessageType.SETTINGS_SOUND, "@SOUND OFF#");
            }
        } else if (buttonView == this.unitsSwitch) {
            if (isChecked) {
                this.callSerialApi(SerialApiMessageType.SETTINGS_METRIC, "@METRIC OFF#");
            } else {
                this.callSerialApi(SerialApiMessageType.SETTINGS_METRIC, "@METRIC ON#");
            }
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.configuration, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == R.id.action_sync_all) {
            this.callSerialApi(SerialApiMessageType.SETTINGS_INFO, "@SETTINGS#");
            return true;
        } else if (id == R.id.action_default) {
            this.callSerialApi(SerialApiMessageType.SETTINGS_DEFAULT, "@DEFAULT#");
            return true;
        } else if (id == R.id.action_auto) {
            this.callSerialApi(SerialApiMessageType.SETTINGS_AUTO, "@AUTO#");
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onClick(View view) {
        if (this.dateEditText == view) {
            this.datePickerDialog.show();
        } else if (this.timeEditText == view) {
            this.timePickerDialog.show();
        } else if (this.nowButton == view) {
            String messageValue = messageDateFormat.format(new Date());
            callSerialApi(SerialApiMessageType.SETTINGS_DATETIME, "@DATETIME " + messageValue + "#");
        }
    }

    private class ConfigurationMessageHandler extends Handler {

        ConfigurationMessageHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case ConfigurationUartGattCallback.SETTINGS_INFO_COMPLETED: {
                    Settings settings = (Settings)msg.obj;
                    this.mapSettingsToScreen(settings);
                } break;
                case ConfigurationUartGattCallback.SETTINGS_DEFAULT_COMPLETED: {
                    Settings settings = (Settings)msg.obj;
                    this.mapSettingsToScreen(settings);
                } break;
                case ConfigurationUartGattCallback.SETTINGS_SOUND_COMPLETED: {
                    soundSwitch.setOnCheckedChangeListener(null);
                    Boolean soundEnabled = (Boolean)msg.obj;
                    soundSwitch.setChecked(soundEnabled);
                    if (soundEnabled) {
                        soundValueTextView.setText(R.string.on);
                    } else {
                        soundValueTextView.setText(R.string.off);
                    }
                    soundSwitch.setOnCheckedChangeListener(ConfigurationBleActivity.this);
                } break;
                case ConfigurationUartGattCallback.SETTINGS_METRIC_COMPLETED: {
                    unitsSwitch.setOnCheckedChangeListener(null);
                    Boolean isImperialUnits = (Boolean)msg.obj;
                    if (isImperialUnits) {
                        unitsValueTextView.setText(R.string.imperial);
                    } else {
                        unitsValueTextView.setText(R.string.metric);
                    }
                    unitsSwitch.setOnCheckedChangeListener(ConfigurationBleActivity.this);
                } break;
                case ConfigurationUartGattCallback.SETTINGS_PRESSURE_COMPLETED: {
                    Double seaLevelPressure = (Double)msg.obj;
                    pressureSeekBar.setProgress(seaLevelPressure.intValue()-pressureSeekBarOffset);
                } break;
                case ConfigurationUartGattCallback.SETTINGS_OXYGEN_COMPLETED: {
                    Double oxygenPercentage = (Double)msg.obj;
                    oxygenPercentage = oxygenPercentage * 1000;
                    oxygenSeekBar.setProgress(oxygenPercentage.intValue()-oxygenSeekBarOffset);
                } break;
                case ConfigurationUartGattCallback.SETTINGS_DATETIME_COMPLETED: {
                    String dateTimeText = (String)msg.obj;

                    Date parsedTime = new Date();
                    try {
                        parsedTime = receivedMessageDateFormat.parse(dateTimeText);
                    } catch (ParseException e) {
                        Log.e(TAG, "Unable to parse DATETIME response: " + dateTimeText, e);
                    }
                    calendar.setTime(parsedTime);

                    dateEditText.setText(currentDateFormat.format(calendar.getTime()));
                    timeEditText.setText(currentTimeFormat.format(calendar.getTime()));
                } break;
                case ConfigurationUartGattCallback.SETTINGS_AUTO_COMPLETED: {
                    Double seaLevelPressure = (Double)msg.obj;
                    pressureSeekBar.setProgress(seaLevelPressure.intValue()-pressureSeekBarOffset);
                } break;
            }
        }

        private void mapSettingsToScreen(Settings settings) {
            //Sound
            soundSwitch.setOnCheckedChangeListener(null);
            Boolean soundEnabled = settings.getSoundEnabled();
            soundSwitch.setChecked(soundEnabled);
            if (soundEnabled) {
                soundValueTextView.setText(R.string.on);
            } else {
                soundValueTextView.setText(R.string.off);
            }
            soundSwitch.setOnCheckedChangeListener(ConfigurationBleActivity.this);

            //Metric
            unitsSwitch.setOnCheckedChangeListener(null);
            Boolean isImperialUnits = settings.getImperialUnits();
            if (isImperialUnits) {
                unitsValueTextView.setText(R.string.imperial);
            } else {
                unitsValueTextView.setText(R.string.metric);
            }
            unitsSwitch.setOnCheckedChangeListener(ConfigurationBleActivity.this);

            //Pressure
            Double seaLevelPressure = settings.getSeaLevelPressure();
            pressureSeekBar.setProgress(seaLevelPressure.intValue()-pressureSeekBarOffset);

            //Oxygen percentage
            Double oxygenPercentage = settings.getOxygenPercentage();
            oxygenPercentage = oxygenPercentage * 1000;
            oxygenSeekBar.setProgress(oxygenPercentage.intValue()-oxygenSeekBarOffset);
        }
    }
}
