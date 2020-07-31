package hu.diveino.droid.ble;

import android.bluetooth.BluetoothGatt;
import android.os.AsyncTask;
import android.util.Log;

import hu.diveino.droid.service.SerialApiMessageType;

public class UartGattAsyncTask extends AsyncTask<String, Void, Void> {

    private static final String TAG = "UartGattAsyncTask";

    private SerialApiMessageType messageType;
    private UartGattCallback uartGattCallback;
    private BluetoothGatt bluetoothGatt;

    public UartGattAsyncTask(SerialApiMessageType messageType, UartGattCallback uartGattCallback, BluetoothGatt bluetoothGatt) {
        this(uartGattCallback, bluetoothGatt);
        this.setMessageType(messageType);
    }

    private UartGattAsyncTask(UartGattCallback uartGattCallback, BluetoothGatt bluetoothGatt) {
        super();
        this.uartGattCallback = uartGattCallback;
        this.bluetoothGatt = bluetoothGatt;
    }

    public void setMessageType(SerialApiMessageType messageType) {
        this.messageType = messageType;
    }

    @Override
    protected Void doInBackground(String... params) {

        int counter = 0;
        while (!this.uartGattCallback.isConnected()) {
            Log.i(TAG, "Waiting for Bluetooth LE connection " + counter*500 + " milliseconds!");
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                Log.e(TAG, e.getLocalizedMessage(), e);
            }
            counter++;
        }

        this.uartGattCallback.send(this.bluetoothGatt, params[0], this.messageType);

        return null;
    }

}
