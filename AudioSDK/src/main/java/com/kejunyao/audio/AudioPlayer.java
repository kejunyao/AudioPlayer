package com.kejunyao.audio;

import android.os.Looper;

import java.io.File;

/**
 * 音频播放控Controller
 *
 * @author kejunyao
 * @since 2020年05月30日
 */
public class AudioPlayer {

    static {
        System.loadLibrary("AudioPlayer");

        System.loadLibrary("avcodec-57");
        System.loadLibrary("avdevice-57");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avformat-57");
        System.loadLibrary("avutil-55");
        System.loadLibrary("postproc-54");
        System.loadLibrary("swresample-2");
        System.loadLibrary("swscale-4");
    }

    private EventHandler mEventHandler;
    private final AudioRecorder mRecorder;


    public AudioPlayer() {
        Looper looper;
        if ((looper = Looper.myLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
        } else if ((looper = Looper.getMainLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
        } else {
            mEventHandler = null;
        }
        mRecorder = new AudioRecorder(this);
    }

    OnPreparedListener mOnPreparedListener;
    public void setOnPreparedListener(OnPreparedListener listener) {
        mOnPreparedListener = listener;
    }

    OnLoadListener mOnLoadListener;
    public void setOnLoadListener(OnLoadListener listener) {
        mOnLoadListener = listener;
    }

    OnPauseResumeListener mOnPauseResumeListener;
    public void setOnPauseResumeListener(OnPauseResumeListener listener) {
        mOnPauseResumeListener = listener;
    }

    OnTimeInfoListener mOnTimeInfoListener;
    public void setOnTimeInfoListener(OnTimeInfoListener listener) {
        mOnTimeInfoListener = listener;
    }

    OnErrorListener mOnErrorListener;
    public void setOnErrorListener(OnErrorListener listener) {
        mOnErrorListener = listener;
    }

    OnCompleteListener mOnCompleteListener;
    public void setOnCompleteListener(OnCompleteListener listener) {
        mOnCompleteListener = listener;
    }

    OnVolumeDecibelListener mOnVolumeDecibelListener;
    public void setOnVolumeDecibelListener(OnVolumeDecibelListener listener) {
        mOnVolumeDecibelListener = listener;
    }

    OnRecordTimeListener mOnRecordTimeListener;
    public void setOnRecordTimeListener(OnRecordTimeListener listener) {
        mOnRecordTimeListener = listener;
    }

    private String mSource;
    public void setDataSource(String source) {
        mSource = source;
    }

    public void prepare() {
        _prepare(mSource);
    }

    public void start() {
        setVolume(sVolumePercent);
        setMute(sMute);
        setSpeed(sSpeed);
        setPitch(sPitch);
        _start();
    }

    public void seek(float percent) {
        _seekByPercent(percent);
    }

    public void seek(int second) {
        _seek(second);
    }

    private static float sVolumePercent = 1.0f;
    public void setVolume(float percent) {
        int value = (int) (100 * percent);
        if (value < 0 || value > 100) {
            return;
        }
        sVolumePercent = percent;
        _setVolume(percent);
    }

    public float getVolumePercent() {
        return sVolumePercent;
    }

    private static Mute sMute = Mute.MUTE_CENTER;
    public void setMute(Mute mute) {
        if (mute == null) {
            return;
        }
        sMute = mute;
        _setMute(mute.ordinal());
    }

    private static float sPitch = 1.0f;
    /**
     * 设置音调
     * @param pitch 音调
     */
    public void setPitch(float pitch) {
        sPitch = pitch;
        _setPitch(pitch);
    }

    public float getPitch() {
        return sPitch;
    }

    private static float sSpeed = 1.0f;
    /**
     * 设置音速
     * @param speed 音速
     */
    public void setSpeed(float speed) {
        sSpeed = speed;
        _setSpeed(speed);
    }

    public float getSpeed() {
        return sSpeed;
    }

    public void resume() {
        _resume();
        if (mEventHandler != null) {
            mEventHandler.sendMessage(EventHandler.EVENT_RESUME);
        }
    }

    public void pause() {
        _pause();
        if (mEventHandler != null) {
            mEventHandler.sendMessage(EventHandler.EVENT_PAUSE);
        }
    }

    private boolean mPlayNext;
    public void playNext(String source) {
        mPlayNext = true;
        mSource = source;
        stop();
    }

    void tryPlayNext() {
        if (mPlayNext) {
            mPlayNext = false;
            prepare();
        }
    }

    public void stop() {
        stopRecord();
        _stop();
        _release();
    }

    private void release() {
        mOnPreparedListener = null;
        mOnLoadListener = null;
        mOnPauseResumeListener = null;
        _release();
    }

    private void postEventFromNative(int what, int arg1, int arg2) {
        if (mEventHandler != null) {
            mEventHandler.sendMessage(what, arg1, arg2);
        }
    }

    private void encodecPcmToAAc(int size, byte[] buffer) {
        mRecorder.encodecPcmToAAc(size, buffer);
    }

    public void startRecord(File out) {
        mRecorder.start(out, _getSampleRate());
    }

    public void stopRecord() {
        mRecorder.stop();
    }

    public void pauseRecord() {
        mRecorder.pause();
    }

    public void resumeRecord() {
        mRecorder.resume();
    }

    void onRecordTime(int time) {
        if (mOnRecordTimeListener != null) {
            postEventFromNative(EventHandler.EVENT_RECORD_TIME, time, 0);
        }
    }

    private native void _prepare(String source);
    private native void _start();
    private native void _seekByPercent(float percent);
    private native void _seek(int second);
    private native void _setVolume(float percent);
    private native void _setMute(int mute);
    private native void _setPitch(float pitch);
    private native void _setSpeed(float speed);
    native void _setRecord(boolean shouldRecord);
    private native int _getSampleRate();
    private native void _pause();
    private native void _resume();
    private native void _stop();
    private native void _release();
    public native boolean isPlaying();

}
