package com.kejunyao.audio.sample;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import com.kejunyao.audio.AudioLog;
import com.kejunyao.audio.AudioPlayer;
import com.kejunyao.audio.Mute;
import com.kejunyao.audio.OnCompleteListener;
import com.kejunyao.audio.OnErrorListener;
import com.kejunyao.audio.OnLoadListener;
import com.kejunyao.audio.OnPauseResumeListener;
import com.kejunyao.audio.OnPreparedListener;
import com.kejunyao.audio.OnTimeInfoListener;
import com.kejunyao.audio.OnVolumeDecibelListener;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "AudioPlayer_MainActivity";

    final AudioPlayer mAudioPlayer = new AudioPlayer();
    private TextView mTimeInfoView;
    private SeekBar mPlayProgressSeekBar;
    private TextView mVolumeTextView;
    private SeekBar mVolumeSeekBar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mPlayProgressSeekBar = findViewById(R.id.seek_bar);
        mVolumeTextView = findViewById(R.id.volume_text);
        mVolumeSeekBar = findViewById(R.id.seek_volume);
        mVolumeSeekBar.setProgress((int) (mAudioPlayer.getVolumePercent() * 100));
        setVolumeProgress(mAudioPlayer.getVolumePercent());

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
                Toast.makeText(MainActivity.this, pause ? "暂停" : "播放", Toast.LENGTH_SHORT).show();
            }
        });
        mAudioPlayer.setOnTimeInfoListener(new OnTimeInfoListener() {
            @Override
            public void onTimeInfo(int currentTime, int totalTime) {
                int progress = (int) (((float) currentTime / (float) totalTime) * mPlayProgressSeekBar.getMax());
                mPlayProgressSeekBar.setProgress(progress);
                mTimeInfoView.setText(TimeUtils.toTimeText(currentTime, totalTime));
                AudioLog.d(TAG, "onTimeInfo, currentTime: ", currentTime, ", totalTime: ", totalTime);
            }
        });
        mAudioPlayer.setOnErrorListener(new OnErrorListener() {
            @Override
            public void onError(int errCode) {
                AudioLog.d(TAG, "错误码：", errCode);
                Toast.makeText(MainActivity.this, "播放错误，errCode: " + errCode, Toast.LENGTH_SHORT).show();
            }
        });

        mPlayProgressSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
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

        mVolumeSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                float percent = (float) mVolumeSeekBar.getProgress() / (float) mVolumeSeekBar.getMax();
                mAudioPlayer.setVolume(percent);
                setVolumeProgress(percent);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });

        mAudioPlayer.setOnCompleteListener(new OnCompleteListener() {
            @Override
            public void onComplete() {
                AudioLog.d(TAG, "播放完成。");
                Toast.makeText(MainActivity.this, "播放完成", Toast.LENGTH_SHORT).show();
            }
        });


        mAudioPlayer.setOnVolumeDecibelListener(new OnVolumeDecibelListener() {
            @Override
            public void onVolumeDecibel(int decibel) {
                AudioLog.d(TAG, "PCM分贝值：", decibel);
            }
        });
    }

    private void setVolumeProgress(float percent) {
        int progress = (int) (100 * percent);
        String text = "音量：" + progress + "%";
        mVolumeTextView.setText(text);
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

    public void muteLeft(View view) {
        mAudioPlayer.setMute(Mute.MUTE_LEFT);
    }

    public void muteRight(View view) {
        mAudioPlayer.setMute(Mute.MUTE_RIGHT);
    }

    public void muteCenter(View view) {
        mAudioPlayer.setMute(Mute.MUTE_CENTER);
    }

    public void speedAdd(View view) {
        mAudioPlayer.setSpeed(mAudioPlayer.getSpeed() + 0.1f);
    }

    public void speedSub(View view) {
        mAudioPlayer.setSpeed(mAudioPlayer.getSpeed() - 0.1f);
    }

    public void pitchAdd(View view) {
        mAudioPlayer.setPitch(mAudioPlayer.getPitch() + 0.1f);
    }

    public void pitchSub(View view) {
        mAudioPlayer.setPitch(mAudioPlayer.getPitch() - 0.1f);
    }


    public void startRecord(View view) {
        mAudioPlayer.startRecord(new File("/sdcard/test789.aac"));
    }

    public void stopRecord(View view) {
        mAudioPlayer.stopRecord();
    }

    public void pauseRecord(View view) {
        mAudioPlayer.pauseRecord();
    }

    public void resumeRecord(View view) {
        mAudioPlayer.resumeRecord();
    }
}
