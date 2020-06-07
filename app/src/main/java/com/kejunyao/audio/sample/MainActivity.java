package com.kejunyao.audio.sample;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import com.kejunyao.audio.AudioLog;
import com.kejunyao.audio.AudioPlayer;
import com.kejunyao.audio.OnCompleteListener;
import com.kejunyao.audio.OnErrorListener;
import com.kejunyao.audio.OnLoadListener;
import com.kejunyao.audio.OnPauseResumeListener;
import com.kejunyao.audio.OnPreparedListener;
import com.kejunyao.audio.OnTimeInfoListener;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "AudioPlayer_MainActivity";

    final AudioPlayer mAudioPlayer = new AudioPlayer();
    private TextView mTimeInfoView;
    private SeekBar mSeekBar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mSeekBar = findViewById(R.id.seek_bar);
        // MediaPlayer mediaPlayer = new MediaPlayer();
        // mediaPlayer.start();
        // mediaPlayer.release();
        // mediaPlayer.pause();
        // mediaPlayer.prepare();
        // mediaPlayer.isPlaying();
        // mediaPlayer.stop();
        // mediaPlayer.release();
        // mediaPlayer.setDataSource("");
        mTimeInfoView = findViewById(R.id.time_info);
        mAudioPlayer.setOnPreparedListener(new OnPreparedListener() {
            @Override
            public void onPrepared() {
                AudioLog.d(TAG, "onPrepared");
                mAudioPlayer.start();
            }
        });
        mAudioPlayer.setOnLoadListener(new OnLoadListener() {
            @Override
            public void onLoad(boolean isLoading) {
                AudioLog.d(TAG, "onLoad, ", (isLoading ? "加载中..." : "加载完成。"));
            }
        });
        mAudioPlayer.setOnPauseResumeListener(new OnPauseResumeListener() {
            @Override
            public void onPause(boolean pause) {
                AudioLog.d(TAG, "onLoad, ", (pause ? "暂停..." : "播放..."));
            }
        });
        mAudioPlayer.setOnTimeInfoListener(new OnTimeInfoListener() {
            @Override
            public void onTimeInfo(int currentTime, int totalTime) {
                mTimeInfoView.setText(TimeUtils.toTimeText(currentTime, totalTime));
                AudioLog.d(TAG, "onTimeInfo, currentTime: ", currentTime, ", totalTime: ", totalTime);
            }
        });
        mAudioPlayer.setOnErrorListener(new OnErrorListener() {
            @Override
            public void onError(int errCode) {
                AudioLog.d(TAG, "错误码：", errCode);
            }
        });

        mSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                float percent = (float) seekBar.getProgress() / (float) seekBar.getMax();
                mAudioPlayer.seek(percent);
            }
        });

        mAudioPlayer.setOnCompleteListener(new OnCompleteListener() {
            @Override
            public void onComplete() {
                AudioLog.d(TAG, "播放完成。");
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

    public void stop(View view) {
        mAudioPlayer.stop();
    }

    public void togglePlayStop(View view) {

    }

    public void seek(View view) {
        mAudioPlayer.seek(200);
    }
}
