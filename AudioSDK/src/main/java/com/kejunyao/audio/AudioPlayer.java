package com.kejunyao.audio;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import androidx.annotation.NonNull;

import java.lang.ref.WeakReference;

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

    public interface OnPreparedListener {
        void onPrepared(AudioPlayer player);
    }

    private class EventHandler extends Handler {
        private final WeakReference<AudioPlayer> mAudioPlayerRef;
        public EventHandler(AudioPlayer player, Looper looper) {
            super(looper);
            mAudioPlayerRef = new WeakReference<>(player);
        }

        @Override
        public void handleMessage(@NonNull Message msg) {
            switch (msg.what) {
                case AUDIO_PREPARED:
                    prepared();
                    break;
                case AUDIO_NOP:
                default: {

                }
            }
        }

        private void prepared() {
            OnPreparedListener preparedListener = mOnPreparedListener;
            if (preparedListener != null) {
                preparedListener.onPrepared(mAudioPlayerRef.get());
            }
        }
    }

    private static final int AUDIO_NOP = 0;
    private static final int AUDIO_PREPARED = 1;

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

    private OnPreparedListener mOnPreparedListener;
    public void setOnPreparedListener(OnPreparedListener listener) {
        mOnPreparedListener = listener;
    }

    private String mSource;
    public void setDataSource(String source) {
        mSource = source;
//        final Uri uri = Uri.parse(path);
//        final String scheme = uri.getScheme();
//        if ("file".equals(scheme)) {
//            return;
//        }
//        if (scheme != null) {
//        }
    }

    public void prepare() {
        _prepare(mSource);
    }

    public void start() {
        _start();
    }

    public void pause() {
        _pause();
    }

    public void resume() {
        _resume();
    }

    public void release() {
        mOnPreparedListener = null;
        _release();
    }

    private void postEventFromNative(int what, int code) {
        if (mEventHandler != null) {
            Message m = mEventHandler.obtainMessage(what, code, 0, null);
            mEventHandler.sendMessage(m);
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
