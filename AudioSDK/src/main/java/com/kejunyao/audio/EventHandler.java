package com.kejunyao.audio;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import androidx.annotation.NonNull;
import java.lang.ref.WeakReference;

/**
 * 音频播放器事件处理Handler
 *
 * @author kejunyao
 * @since 2020年05月31日
 */
class EventHandler extends Handler {

    private static final String TAG = "EventHandler";

    static final int EVENT_NOP       = 0;
    static final int EVENT_PREPARED  = 1;
    static final int EVENT_LOADING   = 2;
    static final int EVENT_PAUSE     = 3;
    static final int EVENT_RESUME    = 4;
    static final int EVENT_TIME_INFO = 5;
    static final int EVENT_ERROR     = 6;

    private final WeakReference<AudioPlayer> mAudioPlayerRef;
    public EventHandler(AudioPlayer player, Looper looper) {
        super(looper);
        mAudioPlayerRef = new WeakReference<>(player);
    }

    @Override
    public void handleMessage(@NonNull Message msg) {
        switch (msg.what) {
            case EVENT_PREPARED:
                onPrepared();
                break;
            case EVENT_LOADING:
                onLoad(msg.arg1 == 0);
                break;
            case EVENT_PAUSE:
            case EVENT_RESUME:
                onPauseResume(msg.what == EVENT_PAUSE);
                break;
            case EVENT_TIME_INFO:
                onTimeInfo(msg.arg1, msg.arg2);
                break;
            case EVENT_NOP:
                break;
            default:
                AudioLog.e(TAG, "Unknown message type ", msg.what);
                break;
        }
    }

    void sendMessage(int what, int arg1, int arg2) {
        Message m = obtainMessage(what, arg1, arg2, null);
        sendMessage(m);
    }

    void sendMessage(int what) {
        sendMessage(what, 0, 0);
    }

    private void onPrepared() {
        AudioPlayer player = mAudioPlayerRef.get();
        if (player != null && player.mOnPreparedListener != null) {
            player.mOnPreparedListener.onPrepared();
        }
    }

    private void onLoad(boolean isLoading) {
        AudioPlayer player = mAudioPlayerRef.get();
        if (player != null && player.mOnLoadListener != null) {
            player.mOnLoadListener.onLoad(isLoading);
        }
    }

    private void onPauseResume(boolean pause) {
        AudioPlayer player = mAudioPlayerRef.get();
        if (player != null && player.mOnPauseResumeListener != null) {
            player.mOnPauseResumeListener.onPause(pause);
        }
    }

    private void onTimeInfo(int currentTime, int totalTime) {
        AudioPlayer player = mAudioPlayerRef.get();
        if (player != null && player.mOnTimeInfoListener != null) {
            player.mOnTimeInfoListener.onTimeInfo(currentTime, totalTime);
        }
    }

}
