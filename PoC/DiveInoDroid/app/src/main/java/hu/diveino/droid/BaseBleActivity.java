package hu.diveino.droid;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import hu.diveino.droid.ble.UartGattAsyncTask;
import hu.diveino.droid.ble.UartGattCallback;
import hu.diveino.droid.service.SerialApiMessageType;

public class BaseBleActivity extends AppCompatActivity {

    private static final String TAG = "BaseBleActivity";

    protected BluetoothDevice bluetoothDevice;
    protected BluetoothGatt bluetoothGatt;
    protected UartGattCallback uartGattCallback;
    protected Handler messageHandler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        this.bluetoothDevice = this.getIntent().getParcelableExtra(HomeActivity.BLUETOOTH_DEVICE_TAG);
    }

    @Override
    public void onStart() {
        super.onStart();

        if (this.bluetoothDevice != null) {
            this.bluetoothGatt = this.bluetoothDevice.connectGatt(this, false, this.uartGattCallback);
            Log.i(TAG, "Bluetooth LE connection was opened.");
        }
    }

    @Override
    public void onStop() {
        super.onStop();

        if (this.bluetoothGatt != null) {
            this.bluetoothGatt.disconnect();
            this.bluetoothGatt.close();
            Log.i(TAG, "Bluetooth LE connection was closed.");
        }
    }

    void callSerialApi(SerialApiMessageType messageType, String message) {
        UartGattAsyncTask uartGattAsyncTask = new UartGattAsyncTask(messageType, this.uartGattCallback, this.bluetoothGatt);
        uartGattAsyncTask.execute(message);
    }

    void callSerialApi(final SerialApiMessageType messageType, final String message, int delayMillis) {
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                callSerialApi(messageType, message);
            }
        }, delayMillis);
    }
}
