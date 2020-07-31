package hu.diveino.droid.util;

public final class TypeConverter {

    private TypeConverter() {}

    /**
     * Convert the given value to a text.
     *
     * If the value is <code>null</code>, return with 0.
     *
     * @param value The value to be converted
     * @return The textual representation of the value. By default 0.
     */
    public static String convertIntegerToText(Integer value) {
        String convertedText = "0";
        if (value != null) {
            convertedText = String.valueOf(value);
        }
        return convertedText;
    }

    /**
     * Convert the given value to a text.
     *
     * If the value is <code>null</code>, return with 0.
     *
     * @param value The value to be converted
     * @return The textual representation of the value. By default 0.
     */
    public static String convertDoubleToText(Double value) {
        String convertedText = "0";
        if (value != null) {
            convertedText = String.valueOf(value);
        }
        return convertedText;
    }

    /**
     * Convert the given value to a <b>two character</b> text.
     *
     * If the value is <code>null</code> return with 00.
     *
     * @param value The value to be converted
     * @return The textual representation of the value. By default 00.
     */
    public static String convertIntegerToTwoCharacterText(Integer value) {
        StringBuffer convertedText = new StringBuffer("");
        if (value != null) {
            if (value < 10) {
                convertedText = convertedText.append("0");
            }
            convertedText = convertedText.append(String.valueOf(value));
        } else {
            convertedText = convertedText.append("00");
        }
        return convertedText.toString();
    }

}
