package com.kejunyao.audio.sample;

import androidx.appcompat.app.AppCompatActivity;

import android.media.MediaPlayer;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import com.kejunyao.audio.AudioPlayer;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "AudioPlayer_MainActivity";

    final AudioPlayer mAudioPlayer = new AudioPlayer();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        // MediaPlayer mediaPlayer = new MediaPlayer();
        // mediaPlayer.start();
        // mediaPlayer.release();
        // mediaPlayer.pause();
        // mediaPlayer.prepare();
        // mediaPlayer.isPlaying();
        // mediaPlayer.stop();
        // mediaPlayer.release();
        // mediaPlayer.setDataSource("");
        mAudioPlayer.setOnPreparedListener(new AudioPlayer.OnPreparedListener() {
            @Override
            public void onPrepared(AudioPlayer player) {
                Log.d(TAG, "onPrepared");
                mAudioPlayer.start();
            }
        });
    }

    public void start(View view) {
        mAudioPlayer.setDataSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
        mAudioPlayer.prepare();
    }

    public void pause(View view) {
        mAudioPlayer.pause();
    }

    public void play(View view) {
        mAudioPlayer.resume();
    }
}
