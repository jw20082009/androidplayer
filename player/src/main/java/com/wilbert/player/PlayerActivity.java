package com.wilbert.player;

import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import static android.opengl.GLES20.GL_COLOR_BUFFER_BIT;

public class PlayerActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.

    JniVideoPlayer videoPlayer;
    GLSurfaceView glSurfaceView;

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
        glSurfaceView.postDelayed(new Runnable() {
            @Override
            public void run() {
                glSurfaceView.requestRender();
            }
        },100);
//        glSurfaceView.requestRender();

    }

    long startTime = 0;

    GLSurfaceView.Renderer renderer = new GLSurfaceView.Renderer() {
        @Override
        public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
            Log.i("CGELOGTAG","onSurfaceCreated");
            GLES20.glClearColor(1.0f, 0f, 0f, 1.0f);
            videoPlayer.initPlayer("/sdcard/DCIM/test.mp4");
            startTime = System.currentTimeMillis();
        }

        @Override
        public void onSurfaceChanged(GL10 gl10, int i, int i1) {
            GLES20.glClearColor(1.0f, 0f, 0f, 1.0f);
        }

        @Override
        public void onDrawFrame(GL10 gl10) {
            GLES20.glClearColor(1.0f, 0f, 0f, 1.0f);
            GLES20.glClear(GL_COLOR_BUFFER_BIT);
//            PlayerActivity.this.onDrawFrame();
            videoPlayer.onDrawFrame(System.currentTimeMillis()- startTime);
        }
    };

    protected void onDrawFrame(){

    }
}
