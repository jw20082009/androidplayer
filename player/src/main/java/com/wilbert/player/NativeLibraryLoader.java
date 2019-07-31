package com.wilbert.player;

/**
 * Created by Android Studio.
 * User: wilbert jw20082009@qq.com
 * Date: 2019/7/31 15:32
 */
public class NativeLibraryLoader {

    private static boolean mLibraryLoaded = false;

    public static void load() {
        if(mLibraryLoaded)
            return;
        mLibraryLoaded = true;
        System.loadLibrary("ffmpeg");
        System.loadLibrary("native-lib");
    }
}
