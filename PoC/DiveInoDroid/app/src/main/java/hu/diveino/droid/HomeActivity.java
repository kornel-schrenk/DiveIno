package hu.diveino.droid;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.NavigationView;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import hu.diveino.droid.util.DrawableUtils;

public class HomeActivity extends AppCompatActivity implements NavigationView.OnNavigationItemSelectedListener, View.OnClickListener {

    private static final String TAG = "HomeActivity";

    private static final int PERMISSION_REQUEST_COARSE_LOCATION = 1;
    public static final String BLUETOOTH_DEVICE_TAG = "BluetoothDevice";

    private static final int REQUEST_ENABLE_BT = 1;
    static final int REQUEST_BLUETOOTH_CONNECT = 2;

    private LinearLayout navigationHeaderLinearLayout;
    private TextView navigationHeaderConnectionTextView;

    private ImageButton connectButton;
    private ImageButton diagnosticsButton;
    private ImageButton configurationButton;
    private ImageButton deviceInformationButton;

    private ImageButton logbookButton;

    private ImageButton settingsButton;
    private ImageButton helpButton;

    private BluetoothAdapter bluetoothAdapter;

    private BluetoothDevice selectedDevice;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_home);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //TODO Add new dive functionality
            }
        });

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(
                this, drawer, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawer.setDrawerListener(toggle);
        toggle.syncState();

        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);

        // Use this check to determine whether BLE is supported on the device.  Then you can selectively disable BLE-related features.
        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            Toast.makeText(this, R.string.ble_not_supported, Toast.LENGTH_SHORT).show();
            //TODO - Turn it back on: finish();
        }

        // Android M Permission check 
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (this.checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION)!=PackageManager.PERMISSION_GRANTED){
                requestPermissions(new String[]{Manifest.permission.ACCESS_COARSE_LOCATION}, PERMISSION_REQUEST_COARSE_LOCATION);
            }
        }

        // Initializes a Bluetooth adapter.  For API level 18 and above, get a reference to BluetoothAdapter through BluetoothManager.
        final BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        this.bluetoothAdapter = bluetoothManager.getAdapter();

        // Checks if Bluetooth is supported on the device.
        if (this.bluetoothAdapter == null) {
            Toast.makeText(this, R.string.bluetooth_not_supported, Toast.LENGTH_SHORT).show();
            //TODO - Turn it back on: finish();
        }

        View header = navigationView.getHeaderView(0);
        this.navigationHeaderLinearLayout = (LinearLayout) header.findViewById(R.id.navHeader_layout);
        this.navigationHeaderConnectionTextView = (TextView) header.findViewById(R.id.navHeader_connectionTextView);

        this.connectButton = (ImageButton) this.findViewById(R.id.home_connect_button);
        this.connectButton.setOnClickListener(this);

        this.diagnosticsButton = (ImageButton) this.findViewById(R.id.home_diagnostics_button);
        this.diagnosticsButton.setOnClickListener(this);
        this.setImageButtonEnabled(false, this.diagnosticsButton, R.drawable.ic_diagnostics);

        this.configurationButton = (ImageButton) this.findViewById(R.id.home_configuration_button);
        this.configurationButton.setOnClickListener(this);
        this.setImageButtonEnabled(false, this.configurationButton, R.drawable.ic_configuration);

        this.deviceInformationButton = (ImageButton) this.findViewById(R.id.home_device_information_button);
        this.deviceInformationButton.setOnClickListener(this);
        this.setImageButtonEnabled(false, this.deviceInformationButton, R.drawable.ic_device_information);

        this.logbookButton = (ImageButton) this.findViewById(R.id.home_logbook_button);
        this.logbookButton.setOnClickListener(this);
        this.setImageButtonEnabled(true, this.logbookButton, R.drawable.ic_logbook);

        this.settingsButton = (ImageButton) this.findViewById(R.id.home_settings_button);
        this.settingsButton.setOnClickListener(this);
        this.setImageButtonEnabled(false, this.settingsButton, R.drawable.ic_settings);

        this.helpButton = (ImageButton) this.findViewById(R.id.home_help_button);
        this.helpButton.setOnClickListener(this);
        this.setImageButtonEnabled(false, this.helpButton, R.drawable.ic_help);
    }

    @Override
    public void onRestoreInstanceState(Bundle savedInstanceState) {
        this.selectedDevice = savedInstanceState.getParcelable(BLUETOOTH_DEVICE_TAG);
        if (this.isConnected()) {
            Log.i(TAG, "Selected device restored: " +
                    this.selectedDevice.getName() + " " + this.selectedDevice.getAddress());
        }
    }

    @Override
    public void onSaveInstanceState(Bundle savedInstanceState) {
        if (this.isConnected()) {
            savedInstanceState.putParcelable(BLUETOOTH_DEVICE_TAG, this.selectedDevice);
            Log.i(TAG, "Selected device stored: " +
                    this.selectedDevice.getName() + " " + this.selectedDevice.getAddress());
        }

        // Always call the superclass so it can save the view hierarchy state
        super.onSaveInstanceState(savedInstanceState);
    }

    @Override
    public void onBackPressed() {
        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        if (drawer.isDrawerOpen(GravityCompat.START)) {
            drawer.closeDrawer(GravityCompat.START);
        } else {
            super.onBackPressed();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        // Ensures Bluetooth is enabled on the device. If Bluetooth is not currently enabled,
        // fire an intent to display a dialog asking the user to grant permission to enable it.
        if (this.bluetoothAdapter != null && !this.bluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.home, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        if (id == R.id.action_exit) {
            disconnect();
            finish();
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @SuppressWarnings("StatementWithEmptyBody")
    @Override
    public boolean onNavigationItemSelected(MenuItem item) {
        // Handle navigation view item clicks here.
        int id = item.getItemId();

        if (id == R.id.nav_computer_diagnostics) {
            if (this.isConnected()) {
                Intent intent = new Intent(HomeActivity.this, DiagnosticsBleActivity.class);
                intent.putExtra(BLUETOOTH_DEVICE_TAG, this.selectedDevice);
                startActivity(intent);
            }
        } else if (id == R.id.nav_computer_configuration) {
            if (this.isConnected()) {
                Intent intent = new Intent(HomeActivity.this, ConfigurationBleActivity.class);
                intent.putExtra(BLUETOOTH_DEVICE_TAG, this.selectedDevice);
                startActivity(intent);
            }
        } else if (id == R.id.nav_device_information) {
            if (this.isConnected()) {
                Intent intent = new Intent(HomeActivity.this, DeviceInformationBleActivity.class);
                intent.putExtra(BLUETOOTH_DEVICE_TAG, this.selectedDevice);
                startActivity(intent);
            }
        } else if (id == R.id.nav_logbook) {
            Intent intent = new Intent(HomeActivity.this, LogbookActivity.class);
            startActivity(intent);
        } else if (id == R.id.nav_settings) {
            //TODO
        } else if (id == R.id.nav_help) {
            //TODO
        }

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        drawer.closeDrawer(GravityCompat.START);
        return true;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
        switch (requestCode) {
            case PERMISSION_REQUEST_COARSE_LOCATION: {
                if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    Log.i(TAG, "COARSE_LOCATION permission granted");
                } else {
                    Toast.makeText(this, R.string.not_granted_permission, Toast.LENGTH_SHORT).show();
                    finish();
                }
            }
        }
    }

    @Override
    public void onClick(View view) {
        if (this.connectButton == view) {
            if (!this.isConnected()) {
                startActivityForResult(new Intent(HomeActivity.this, BleDeviceListActivity.class), REQUEST_BLUETOOTH_CONNECT);
            } else {
                this.disconnect();
            }
        } else if (this.diagnosticsButton == view) {
            if (this.isConnected()) {
                Intent intent = new Intent(HomeActivity.this, DiagnosticsBleActivity.class);
                intent.putExtra(BLUETOOTH_DEVICE_TAG, this.selectedDevice);
                startActivity(intent);
            }
        } else if (this.configurationButton == view) {
            if (this.isConnected()) {
                Intent intent = new Intent(HomeActivity.this, ConfigurationBleActivity.class);
                intent.putExtra(BLUETOOTH_DEVICE_TAG, this.selectedDevice);
                startActivity(intent);
            }
        } else if (this.deviceInformationButton == view) {
            if (this.isConnected()) {
                Intent intent = new Intent(HomeActivity.this, DeviceInformationBleActivity.class);
                intent.putExtra(BLUETOOTH_DEVICE_TAG, this.selectedDevice);
                startActivity(intent);
            }
        } else if (this.logbookButton == view) {
            Intent intent = new Intent(HomeActivity.this, LogbookActivity.class);
            startActivity(intent);
        } else if (this.settingsButton == view) {
            //TODO
        } else if (this.helpButton == view) {
            //TODO
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (REQUEST_BLUETOOTH_CONNECT == requestCode && RESULT_OK == resultCode) {
            BluetoothDevice bluetoothDevice = data.getParcelableExtra(BLUETOOTH_DEVICE_TAG);
            this.connect(bluetoothDevice);
        }
    }

    private boolean isConnected() {
        return this.selectedDevice != null;
    }

    private void connect(BluetoothDevice bluetoothDevice) {
        if (bluetoothDevice != null) {
            this.selectedDevice = bluetoothDevice;
            Log.i(TAG, "Selected device address: " + this.selectedDevice.getAddress());

            this.navigationHeaderLinearLayout.setBackground(DrawableUtils.getDrawableForResource(R.drawable.connected, this));
            this.navigationHeaderConnectionTextView.setText(R.string.connected);

            this.setImageButtonEnabled(true, this.diagnosticsButton, R.drawable.ic_diagnostics);
            this.setImageButtonEnabled(true, this.configurationButton, R.drawable.ic_configuration);
            this.setImageButtonEnabled(true, this.deviceInformationButton, R.drawable.ic_device_information);

            this.connectButton.setImageDrawable(DrawableUtils.getDrawableForResource(R.drawable.ic_disconnect, this));
        }
    }

    private void disconnect() {
        this.selectedDevice = null;

        this.navigationHeaderLinearLayout.setBackground(DrawableUtils.getDrawableForResource(R.drawable.disconnected, this));
        this.navigationHeaderConnectionTextView.setText(R.string.disconnected);

        this.setImageButtonEnabled(false, this.diagnosticsButton, R.drawable.ic_diagnostics);
        this.setImageButtonEnabled(false, this.configurationButton, R.drawable.ic_configuration);
        this.setImageButtonEnabled(false, this.deviceInformationButton, R.drawable.ic_device_information);

        this.connectButton.setImageDrawable(DrawableUtils.getDrawableForResource(R.drawable.ic_connect, this));
    }

    private void setImageButtonEnabled(boolean enabled, ImageButton button, int iconResId) {
        button.setEnabled(enabled);

        Drawable originalIcon = DrawableUtils.getDrawableForResource(iconResId, this);
        if (enabled) {
            button.setImageDrawable(originalIcon);
        } else {
            Drawable icon = originalIcon.mutate();
            icon.setColorFilter(Color.GRAY, PorterDuff.Mode.SRC_IN);
            button.setImageDrawable(icon);
        }
    }
}
