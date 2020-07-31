package hu.diveino.droid.ble;

import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import java.nio.charset.Charset;
import java.util.Arrays;
import java.util.UUID;

import hu.diveino.droid.service.SerialApiMessageType;
import hu.diveino.droid.util.ByteUtils;

public abstract class UartGattCallback extends BluetoothGattCallback {

    private static final String TAG = "UartGattCallback";

    public static final int UART_TX_MAX_CHARACTERS = 20;

    // UUIDs for UART service and associated characteristics.
    public static UUID UART_UUID = UUID.fromString("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
    public static UUID TX_UUID   = UUID.fromString("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
    public static UUID RX_UUID   = UUID.fromString("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");

    // UUID for the UART BTLE client characteristic which is necessary for notifications.
    public static UUID CLIENT_UUID = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");

    public static final int MESSAGE_ERROR_COMPLETED = -1;

    private BluetoothGattCharacteristic tx;
    private BluetoothGattCharacteristic rx;

    private Handler replyMessageHandler;
    private SerialApiMessageType messageType = SerialApiMessageType.DIAG_PRES;

    private StringBuffer receiveBuffer = new StringBuffer();

    private volatile boolean connected = false;

    public UartGattCallback(Handler replyMessageHandler) {
        super();
        this.replyMessageHandler = replyMessageHandler;
    }

    public Handler getReplyMessageHandler() {
        return this.replyMessageHandler;
    }

    protected SerialApiMessageType getMessageType() {
        return this.messageType;
    }

    @Override
    public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
        super.onConnectionStateChange(gatt, status, newState);
        if (newState == BluetoothGatt.STATE_CONNECTED) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                // Connected to device, start discovering services.
                if (!gatt.discoverServices()) {
                    // Error starting service discovery.
                    connectFailure();
                }
            }
            else {
                connectFailure();
            }
        }
        else if (newState == BluetoothGatt.STATE_DISCONNECTED) {
            rx = null;
            tx = null;
            this.connected = false;
            Log.i(TAG, "Bluetooth LE UART service was disconnected.");

            //TODO BLE UART disconnected, notify callbacks of disconnection.
        }
    }

    @Override
    public void onServicesDiscovered(BluetoothGatt gatt, int status) {
        super.onServicesDiscovered(gatt, status);
        // Notify connection failure if service discovery failed.
        if (status == BluetoothGatt.GATT_FAILURE) {
            connectFailure();
            return;
        }

        // Save reference to each UART characteristic.
        tx = gatt.getService(UART_UUID).getCharacteristic(TX_UUID);
        rx = gatt.getService(UART_UUID).getCharacteristic(RX_UUID);

        // Setup notifications on RX characteristic changes (i.e. data received).
        // First call setCharacteristicNotification to enable notification.
        if (!gatt.setCharacteristicNotification(rx, true)) {
            // Stop if the characteristic notification setup failed.
            connectFailure();
            return;
        }

        // Next update the RX characteristic's client descriptor to enable notifications.
        BluetoothGattDescriptor desc = rx.getDescriptor(CLIENT_UUID);
        if (desc == null) {
            // Stop if the RX characteristic has no client descriptor.
            connectFailure();
            return;
        }
        desc.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
        if (!gatt.writeDescriptor(desc)) {
            // Stop if the client descriptor could not be written.
            connectFailure();
            return;
        }
        // Notify of connection completion.
        this.connected = true;
        Log.i(TAG, "Bluetooth LE UART service was connected.");
    }

    @Override
    public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
        super.onCharacteristicWrite(gatt, characteristic, status);
    }

    @Override
    public void onCharacteristicRead (BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
        super.onCharacteristicRead(gatt, characteristic, status);
    }

    @Override
    public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
        super.onCharacteristicChanged(gatt, characteristic);

        byte[] bytes = characteristic.getValue();
        if (bytes.length > 0) {
            final String response = new String(bytes, Charset.forName("UTF-8"));

            if (ByteUtils.contains(bytes, (byte)64)) { //@ - START
                this.receiveBuffer = new StringBuffer();
                this.receiveBuffer = this.receiveBuffer.append(response);
                this.receiveBuffer = this.receiveBuffer.deleteCharAt(0);
            } else {
                this.receiveBuffer = this.receiveBuffer.append(response);
            }

            if (ByteUtils.contains(bytes, (byte)35)) { //# - END
                String completeMessage = this.receiveBuffer.substring(0, this.receiveBuffer.lastIndexOf("#"));
                Log.i(TAG, "Received complete message for " + this.messageType + ":\n" + completeMessage);

                sendMessage(this.getReplyMessageHandler().obtainMessage(
                        this.getResponseMessageType(),
                        this.parseResponseValue(completeMessage)));
            }
        }
    }

    public abstract Object parseResponseValue(String responseValue);

    public abstract int getResponseMessageType();

    public boolean isConnected() {
        return this.connected;
    }

    private void connectFailure() {
        rx = null;
        tx = null;
        this.connected = false;
        Log.e(TAG, "Bluetooth LE connection failure.");

        //TODO Notify callbacks of connection failure, and reset connection state.
    }

    public void send(BluetoothGatt gatt, String data, SerialApiMessageType serialApiMessageType) {
        if (data != null && !data.isEmpty()) {
            this.messageType = serialApiMessageType;
            send(gatt, data.getBytes(Charset.forName("UTF-8")));
        }
    }

    public void send(BluetoothGatt gatt, byte[] data, SerialApiMessageType serialApiMessageType) {
        if (data != null && data.length > 0) {
            this.messageType = serialApiMessageType;
            send(gatt, data);
        }
    }

    private void send(BluetoothGatt gatt, byte[] data) {
        if (!connected || tx == null || data == null || data.length == 0) {
            Log.w(TAG, "Failed to send message: no Bluetooth LE connection available or empty message.");
            return;
        }

        // Message has to be sent in chunks, because there is a UART_TX_MAX_CHARACTERS on the UART TX channel
        for (int i = 0; i < data.length; i += UART_TX_MAX_CHARACTERS) {
            final byte[] chunk = Arrays.copyOfRange(data, i, Math.min(i + UART_TX_MAX_CHARACTERS, data.length));
            tx.setValue(chunk);
            gatt.writeCharacteristic(tx);

            try {
                //Small break is necessary otherwise the UART connection doesn't notice it as another chunk
                Thread.sleep(100);
            } catch (InterruptedException e) {
                Log.e(TAG, "Error during data send." , e);
            }
        }
        Log.i(TAG, "Bluetooth LE UART message sent: " + new String(data));
    }

    void sendMessage(Message message) {
        this.replyMessageHandler.sendMessage(message);
    }
}
