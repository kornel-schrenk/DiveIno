package hu.diveino.droid.ble;

import android.bluetooth.BluetoothDevice;
import android.util.Log;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.UUID;

public class BluetoothDeviceData {

    private static final String TAG = "BluetoothDeviceData";

    public static final String UART_UUID_SERVICE = "6e400001-b5a3-f393-e0a9-e50e24dcca9e";

    // Decoded scan record (update R.array.scan_devicetypes if this list is modified)
    public static final int TYPE_UNKNOWN = 0;
    public static final int TYPE_UART = 1;
    public static final int TYPE_BEACON = 2;
    public static final int TYPE_URI_BEACON = 3;

    private BluetoothDevice device;
    private int rssi;
    private byte[] scanRecord;

    private String advertisedName;
    private int type;
    private int txPower;
    private ArrayList<UUID> uuids;

    public BluetoothDeviceData(BluetoothDevice device, int rssi, byte[] scanRecord) {
        this.device = device;
        this.rssi = rssi;
        this.scanRecord = scanRecord;

        //Run the decode operation
        this.decodeScanRecords();
    }

    public BluetoothDevice getDevice() {
        return this.device;
    }

    public String getName() {
        String name = device.getName();
        if (name == null) {
            name = advertisedName;      // Try to get a name (but it seems that if device.getName() is null, this is also null)
        }
        return name;
    }

    public String getAddress() {
        return this.device.getAddress();
    }

    public int getRssi() {
        return this.rssi;
    }

    public int getType() {
        return this.type;
    }

    public int getTxPower() {
        return this.txPower;
    }

    public List getUuids() {
        return this.uuids;
    }

    // based on http://stackoverflow.com/questions/24003777/read-advertisement-packet-in-android
    private void decodeScanRecords() {
        this.uuids = new ArrayList<>();
        byte[] advertisedData = Arrays.copyOf(scanRecord, scanRecord.length);
        int offset = 0;
        this.type = TYPE_UNKNOWN;

        // Check if is an iBeacon ( 0x02, 0x0x1, a flag byte, 0x1A, 0xFF, manufacturer (2bytes), 0x02, 0x15)
        final boolean isBeacon = advertisedData[0] == 0x02 && advertisedData[1] == 0x01 && advertisedData[3] == 0x1A && advertisedData[4] == (byte) 0xFF && advertisedData[7] == 0x02 && advertisedData[8] == 0x15;

        // Check if is an URIBeacon
        final byte[] kUriBeaconPrefix = {0x03, 0x03, (byte) 0xD8, (byte) 0xFE};
        final boolean isUriBeacon = Arrays.equals(Arrays.copyOf(scanRecord, kUriBeaconPrefix.length), kUriBeaconPrefix) && advertisedData[5] == 0x16 && advertisedData[6] == kUriBeaconPrefix[2] && advertisedData[7] == kUriBeaconPrefix[3];

        if (isBeacon) {
            this.type = TYPE_BEACON;

            // Read uuid
            offset = 9;
            UUID uuid = this.getUuidFromByteArrayBigEndian(Arrays.copyOfRange(scanRecord, offset, offset + 16));
            uuids.add(uuid);
            offset += 16;

            // Skip major minor
            offset += 2 * 2;   // major, minor

            // Read txpower
            final int txPower = advertisedData[offset++];
            this.txPower = txPower;
        } else if (isUriBeacon) {
            this.type = TYPE_URI_BEACON;

            // Read txpower
            final int txPower = advertisedData[9];
            this.txPower = txPower;
        } else {
            // Read standard advertising packet
            while (offset < advertisedData.length - 2) {
                // Length
                int len = advertisedData[offset++];
                if (len == 0) break;

                // Type
                int type = advertisedData[offset++];
                if (type == 0) break;

                // Data
//            Log.d(TAG, "record -> lenght: " + length + " type:" + type + " data" + data);

                switch (type) {
                    case 0x02:          // Partial list of 16-bit UUIDs
                    case 0x03: {        // Complete list of 16-bit UUIDs
                        while (len > 1) {
                            int uuid16 = advertisedData[offset++] & 0xFF;
                            uuid16 |= (advertisedData[offset++] << 8);
                            len -= 2;
                            uuids.add(UUID.fromString(String.format("%08x-0000-1000-8000-00805f9b34fb", uuid16)));
                        }
                        break;
                    }

                    case 0x06:          // Partial list of 128-bit UUIDs
                    case 0x07: {        // Complete list of 128-bit UUIDs
                        while (len >= 16) {
                            try {
                                // Wrap the advertised bits and order them.
                                UUID uuid = this.getUuidFromByteArraLittleEndian(Arrays.copyOfRange(advertisedData, offset, offset + 16));
                                uuids.add(uuid);

                            } catch (IndexOutOfBoundsException e) {
                                Log.e(TAG, "BlueToothDeviceFilter.parseUUID: " + e.toString());
                            } finally {
                                // Move the offset to read the next uuid.
                                offset += 16;
                                len -= 16;
                            }
                        }
                        break;
                    }

                    case 0x09: {
                        byte[] nameBytes = new byte[len - 1];
                        for (int i=0; i<len-1; i++) {
                            nameBytes[i] = advertisedData[offset++];
                        }

                        String name = null;
                        try {
                            name = new String(nameBytes, "utf-8");
                        } catch (UnsupportedEncodingException e) {
                            e.printStackTrace();
                        }
                        this.advertisedName = name;
                        break;
                    }

                    case 0x0A: {        // TX Power
                        final int txPower = advertisedData[offset++];
                        this.txPower = txPower;
                        break;
                    }

                    default: {
                        offset += (len - 1);
                        break;
                    }
                }
            }

            // Check if Uart is contained in the uuids
            boolean isUart = false;
            for (UUID uuid : uuids) {
                if (uuid.toString().equalsIgnoreCase(UART_UUID_SERVICE)) {
                    isUart = true;
                    break;
                }
            }
            if (isUart) {
                this.type = TYPE_UART;
            }
        }
    }

    private UUID getUuidFromByteArrayBigEndian(byte[] bytes) {
        ByteBuffer bb = ByteBuffer.wrap(bytes);
        long high = bb.getLong();
        long low = bb.getLong();
        UUID uuid = new UUID(high, low);
        return uuid;
    }

    private UUID getUuidFromByteArraLittleEndian(byte[] bytes) {
        ByteBuffer bb = ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN);
        long high = bb.getLong();
        long low = bb.getLong();
        UUID uuid = new UUID(low, high);
        return uuid;
    }
}
