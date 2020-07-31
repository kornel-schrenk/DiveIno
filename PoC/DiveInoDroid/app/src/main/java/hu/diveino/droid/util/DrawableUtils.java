package hu.diveino.droid.util;

import android.app.Activity;
import android.graphics.drawable.Drawable;

public final class DrawableUtils {

    public static Drawable getDrawableForResource(int id, Activity context) {
        Drawable myDrawable;
        if(android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP){
            myDrawable = context.getResources().getDrawable(id, context.getTheme());
        } else {
            myDrawable = context.getResources().getDrawable(id);
        }
        return myDrawable;
    }
}
