package com.wilbert.androidplayer;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.wilbert.player.PlayerActivity;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    Button buttonPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        buttonPlayer = findViewById(R.id.btn_player);
        buttonPlayer.setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.btn_player:
                startActivity(new Intent(this, PlayerActivity.class));
                break;
        }
    }
}
