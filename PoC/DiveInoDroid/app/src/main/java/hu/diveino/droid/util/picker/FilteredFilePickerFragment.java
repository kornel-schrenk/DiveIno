package hu.diveino.droid.util.picker;

import android.support.annotation.NonNull;

import com.nononsenseapps.filepicker.FilePickerFragment;

import java.io.File;

public class FilteredFilePickerFragment extends FilePickerFragment {

    private static final String FILE_PREFIX = "Dive";
    private static final String JSON_EXTENSION = ".json";

    /**
     * Returns with the extension of the given file.
     *
     * @param file The File object
     * @return The file extension. If file has no extension, it returns null.
     */
    private String getExtension(@NonNull File file) {
        String path = file.getPath();
        int i = path.lastIndexOf(".");
        if (i < 0) {
            return null;
        } else {
            return path.substring(i);
        }
    }

    @Override
    protected boolean isItemVisible(final File file) {
        boolean ret = super.isItemVisible(file);
        if (ret && !isDir(file) && (mode == MODE_FILE || mode == MODE_FILE_AND_DIR)) {
            String extension = getExtension(file);
            return file.getName().contains(FILE_PREFIX) &&
                   extension != null && JSON_EXTENSION.equalsIgnoreCase(extension);
        }
        return ret;
    }
}