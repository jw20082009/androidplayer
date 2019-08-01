package com.wilbert.player;

import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import java.lang.ref.SoftReference;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class PlayerActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.

    JniVideoPlayer videoPlayer;
    GLSurfaceView glSurfaceView;
    MyHandler handler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_player);
        videoPlayer = new JniVideoPlayer();
        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(videoPlayer.stringFromJNI());
        glSurfaceView = findViewById(R.id.surfaceview);
        glSurfaceView.setEGLContextClientVersion(2);
        glSurfaceView.setRenderer(renderer);
        glSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        handler = new MyHandler(this);
        handler.sendEmptyMessage(MSG_REQUEST_FRAME);
    }

    @Override
    protected void onResume() {
        super.onResume();
        glSurfaceView.onResume();
    }

    @Override
    protected void onPause() {
        handler.removeMessages(MSG_REQUEST_FRAME);
        glSurfaceView.queueEvent(new Runnable() {
            @Override
            public void run() {
                videoPlayer.release();
            }
        });
        super.onPause();
    }

    long startTime = 0;
    int surfaceWidth, surfaceHeight;

    GLSurfaceView.Renderer renderer = new GLSurfaceView.Renderer() {
        @Override
        public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
            Log.i("CGELOGTAG", "onSurfaceCreated");
            GLES20.glClearColor(1.0f, 0f, 0f, 1.0f);
            videoPlayer.initPlayer("/sdcard/DCIM/test.mp4");
            startTime = System.currentTimeMillis();
        }

        @Override
        public void onSurfaceChanged(GL10 gl10, int i, int i1) {
            surfaceWidth = i;
            surfaceHeight = i1;
            GLES20.glClearColor(1.0f, 0f, 0f, 1.0f);
        }

        @Override
        public void onDrawFrame(GL10 gl10) {
            GLES20.glViewport(0, 0, surfaceWidth, surfaceHeight);
            videoPlayer.renderer();
            videoPlayer.update(System.currentTimeMillis() - startTime);
        }
    };

    class MyHandler extends Handler {
        SoftReference<PlayerActivity> reference;

        public MyHandler(PlayerActivity activity) {
            reference = new SoftReference<>(activity);
        }

        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            if (reference != null) {
                reference.get().handleMessage(msg);
            }
        }

        public void invalidate() {
            removeMessages(MSG_REQUEST_FRAME);
            removeCallbacks(null);
            reference.clear();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        handler.invalidate();
    }

    private final int MSG_REQUEST_FRAME = 0X01;

    protected void handleMessage(Message message) {
        switch (message.what) {
            case MSG_REQUEST_FRAME:
                glSurfaceView.requestRender();
                handler.sendEmptyMessageDelayed(MSG_REQUEST_FRAME, (long) (1.0f * 1000 / 30));
                break;
        }
    }
}
