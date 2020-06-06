package com.kejunyao.audio;

import android.os.Looper;

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

    public AudioPlayer() {
        Looper looper;
        if ((looper = Looper.myLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
        } else if ((looper = Looper.getMainLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
        } else {
            mEventHandler = null;
        }
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

    private String mSource;
    public void setDataSource(String source) {
        mSource = source;
    }

    public void prepare() {
        _prepare(mSource);
    }

    public void start() {
        _start();
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

    public void stop() {
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

    private native void _prepare(String source);
    private native void _start();
    private native void _pause();
    private native void _resume();
    private native void _stop();
    private native void _release();
    public native boolean isPlaying();


}
