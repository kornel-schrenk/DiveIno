package hu.diveino.droid.util;

public final class ByteUtils {

    /**
     * Returns with <code>true</code>, if the value is in the byte array.
     *
     * @param array
     * @param value
     * @return
     */
    public static boolean contains(byte[] array, byte value) {
        for (byte index : array) {
            if (index == value) {
                return true;
            }
        }
        return false;
    }

    /**
     * Returns the first index of the given byte value in the array.
     *
     * @param array
     * @param value
     * @return -1, if the value is not in the array
     */
    public static int indexOf(byte[] array, byte value) {
        for (int i=0; i<array.length; i++) {
            if (array[i] == value) {
                return i;
            }
        }
        return -1;
    }

    /**
     * Returns the remainder byte array from the given index. The byte value on the
     * given index is <b>not</b> part of the result array.
     *
     * Returns an empty array, if the index is not within the array or it points to
     * the last element in the array.
     *
     * @param array
     * @param fromIndex
     * @return
     */
    public static byte[] subByteArray(byte[] array, int fromIndex) {
        if (fromIndex >= 0 && fromIndex < array.length-1) {
            byte[] output = new byte[array.length-fromIndex-1];
            System.arraycopy(array, fromIndex+1, output, 0, array.length-fromIndex-1);
            return output;
        }
        return new byte[0];
    }

    public static byte[] truncByteArray(byte[] array, int size) {
        if (size <= array.length) {
            byte[] output = new byte[size];
            System.arraycopy(array, 0, output, 0, size);
            return output;
        }
        return array;
    }
}
