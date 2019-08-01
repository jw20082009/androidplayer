package com.wilbert.player;

/**
 * Created by Android Studio.
 * User: wilbert jw20082009@qq.com
 * Date: 2019/7/30 17:14
 */
public class JniVideoPlayer {

    public JniVideoPlayer() {
        NativeLibraryLoader.load();
    }

    public native String stringFromJNI();

    public native void renderer();

    public native void update(long timestamp);

    public native void initPlayer(String filepath);

    public native void release();
}
