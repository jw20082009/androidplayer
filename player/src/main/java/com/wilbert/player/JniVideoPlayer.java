package com.wilbert.player;

/**
 * Created by Android Studio.
 * User: wilbert jw20082009@qq.com
 * Date: 2019/7/30 17:14
 */
public class JniVideoPlayer {

    static {
        System.loadLibrary("native-lib");
    }

    public native String stringFromJNI();

    public native void onDrawFrame();
}