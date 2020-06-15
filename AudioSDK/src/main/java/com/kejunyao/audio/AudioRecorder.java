package com.kejunyao.audio;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;

import java.io.File;
import java.io.FileOutputStream;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.util.Arrays;

import androidx.annotation.NonNull;

/**
 * 音频录制器
 *
 * @author kejunyao
 * @since 2020年05月30日
 */
final class AudioRecorder {

    private static final String TAG = "AudioRecorder";

    private static final int MAX_BUFFER_SIZE = 4096;

    private MediaFormat encoderFormat;
    private MediaCodec encoder;
    private FileOutputStream outputStream;
    private MediaCodec.BufferInfo info;
    private int perPCMSize = 0;
    private byte[] outByteBuffer;
    private int aacSampleRate = 4;
    private boolean sInit;
    private int audioSampleRate;
    private double recordTime;

    private final AudioPlayer mPlayer;

    private HandlerThread mHandlerThread;
    private RecorderHandler mHandler;

    public AudioRecorder(AudioPlayer player) {
        mPlayer = player;
    }

    private void startRecorderHandler() {
        if (mHandlerThread == null) {
            mHandlerThread = new HandlerThread("AudioRecorder_Handler");
            mHandlerThread.start();
            mHandler = new RecorderHandler(mHandlerThread.getLooper(), this);
        }
    }

    private void releaseRecorderHandler() {
        if (mHandlerThread != null) {
            mHandlerThread.quit();
            mHandlerThread = null;
            mHandler = null;
        }
    }

    private boolean canSendMessage() {
        return mHandlerThread != null && mHandler != null;
    }

    private void init() {
        try {
            aacSampleRate = AudioPlayerUtils.getADTSSampleRate(audioSampleRate);
            encoderFormat = MediaFormat.createAudioFormat(MediaFormat.MIMETYPE_AUDIO_AAC, audioSampleRate, 2);
            encoderFormat.setInteger(MediaFormat.KEY_BIT_RATE, 96000);
            encoderFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);
            encoderFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, MAX_BUFFER_SIZE);
            encoder = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_AUDIO_AAC);
            info = new MediaCodec.BufferInfo();
            if (encoder == null) {
                return;
            }
            recordTime = 0;
            encoder.configure(encoderFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
            outputStream = new FileOutputStream(mFile);
            encoder.start();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    void encodecPcmToAAc(int size, byte[] buffer) {
        if (!canSendMessage()) {
            return;
        }
        Message message = mHandler.obtainMessage();
        message.what = RecorderHandler.MSG_PCM_TO_AAC;
        message.obj = buffer;
        message.arg1 = size;
        message.sendToTarget();
    }

    private void handlePCMToAAC(int size, byte[] buffer) {
        if (size > MAX_BUFFER_SIZE) {
            int num = size / MAX_BUFFER_SIZE;
            for (int i = 0; i < num; i++) {
                int from = i * MAX_BUFFER_SIZE;
                int to = from + MAX_BUFFER_SIZE;
                byte[] subBuffer = Arrays.copyOfRange(buffer, from, to);
                pcmToAac(MAX_BUFFER_SIZE, subBuffer);
            }
            int last = size % MAX_BUFFER_SIZE;
            if (last > 0) {
                int from = num * MAX_BUFFER_SIZE;
                int to = from + last;
                byte[] subBuffer = Arrays.copyOfRange(buffer, from, to);
                pcmToAac(MAX_BUFFER_SIZE, subBuffer);
            }
        } else {
            pcmToAac(size, buffer);
        }
    }

    private void pcmToAac(int size, byte[] buffer) {
        if (buffer != null && encoder != null) {
            recordTime += size * 1.0 / (audioSampleRate * 2 * (16 / 8));
            mPlayer.onRecordTime((int) recordTime);
            int inputBufferIndex = encoder.dequeueInputBuffer(0);
            if (inputBufferIndex >= 0) {
                ByteBuffer byteBuffer = encoder.getInputBuffers()[inputBufferIndex];
                byteBuffer.clear();
                byteBuffer.put(buffer);
                encoder.queueInputBuffer(inputBufferIndex, 0, size, 0, 0);
            }

            int index = encoder.dequeueOutputBuffer(info, 0);
            while (index >= 0) {
                try {
                    perPCMSize = info.size + 7;
                    outByteBuffer = new byte[perPCMSize];

                    ByteBuffer byteBuffer = encoder.getOutputBuffers()[index];
                    byteBuffer.position(info.offset);
                    byteBuffer.limit(info.offset + info.size);

                    AudioPlayerUtils.addADtsHeader(outByteBuffer, perPCMSize, aacSampleRate);

                    byteBuffer.get(outByteBuffer, 7, info.size);
                    byteBuffer.position(info.offset);
                    outputStream.write(outByteBuffer, 0, perPCMSize);

                    encoder.releaseOutputBuffer(index, false);
                    index = encoder.dequeueOutputBuffer(info, 0);
                    outByteBuffer = null;
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }

    private void release() {
        if (!canSendMessage()) {
            return;
        }
        sendMessage(RecorderHandler.MSG_RELEASE);
    }

    private void handleRelease() {
        if (encoder == null) {
            return;
        }
        recordTime = 0;
        try {
            outputStream.close();
            outputStream = null;
        } catch (Exception e) {
        } finally {
            if (outputStream != null) {
                try {
                    outputStream.close();
                } catch (Exception e) {
                } finally {
                    outputStream = null;
                }
            }
        }
        try {
            encoder.stop();
            encoder.release();
        } catch (Exception e) {
        }
        encoder = null;
        encoderFormat = null;
        info = null;
        sInit = false;
        releaseRecorderHandler();
        AudioLog.d(TAG, "录制完成。");
    }

    void start(File file, int sampleRate) {
        startRecorderHandler();
        Message msg = mHandler.obtainMessage();
        msg.what = RecorderHandler.MSG_START;
        msg.arg1 = sampleRate;
        msg.obj = file;
        msg.sendToTarget();
    }

    private File mFile;

    private void handleStart(File file, int sampleRate) {
        if (mFile != null && mFile.getAbsolutePath().equals(file.getAbsolutePath())) {
            return;
        }
        mFile = file;
        if (!sInit) {
            audioSampleRate = sampleRate;
            if (sampleRate > 0) {
                sInit = true;
                init();
                mPlayer._setRecord(true);
                AudioLog.d(TAG, "开始录制, sampleRate: ", sampleRate);
                return;
            }
        }
    }

    void stop() {
        if (!canSendMessage()) {
            return;
        }
        sendMessage(RecorderHandler.MSG_STOP);
    }

    private void handleStop() {
        if (sInit) {
            mPlayer._setRecord(false);
            release();
            sInit = false;
            AudioLog.d(TAG, "完成录制");
        }
    }

    void pause() {
        if (!canSendMessage()) {
            return;
        }
        sendMessage(RecorderHandler.MSG_PAUSE);
    }

    private void handlePause() {
        mPlayer._setRecord(false);
        AudioLog.d(TAG, "暂停录制");
    }

    void resume() {
        if (!canSendMessage()) {
            return;
        }
        sendMessage(RecorderHandler.MSG_RESUME);
    }

    private void handleResume() {
        mPlayer._setRecord(true);
        AudioLog.d(TAG, "继续录制");
    }

    private void sendMessage(int what) {
        Message message = mHandler.obtainMessage();
        message.what = what;
        message.sendToTarget();
    }

    private static class RecorderHandler extends Handler {

        private static final int MSG_START      = 1;
        private static final int MSG_PCM_TO_AAC = 2;
        private static final int MSG_PAUSE      = 3;
        private static final int MSG_RESUME     = 4;
        private static final int MSG_STOP       = 5;
        private static final int MSG_RELEASE    = 6;

        private final WeakReference<AudioRecorder> playerReference;

        public RecorderHandler(@NonNull Looper looper, AudioRecorder recorder) {
            super(looper);
            playerReference = new WeakReference<>(recorder);
        }

        @Override
        public void handleMessage(@NonNull Message msg) {
            switch (msg.what) {
                case MSG_START: {
                    start(msg);
                    break;
                }
                case MSG_PCM_TO_AAC: {
                    pcmToAAC(msg);
                    break;
                }
                case MSG_PAUSE: {
                    pause();
                    break;
                }
                case MSG_RESUME: {
                    resume();
                    break;
                }
                case MSG_STOP: {
                    stop();
                    break;
                }
                case MSG_RELEASE: {
                    release();
                    break;
                }
                default:
                    AudioLog.e(TAG, "unknow msg what: ", msg.what);
                    break;
            }
        }

        private void start(Message msg) {
            AudioRecorder recorder = playerReference.get();
            if (recorder != null && (msg.obj instanceof File)) {
                recorder.handleStart((File) msg.obj, msg.arg1);
            }
        }

        private void pcmToAAC(Message msg) {
            AudioRecorder recorder = playerReference.get();
            if (recorder != null && (msg.obj instanceof byte[])) {
                recorder.handlePCMToAAC(msg.arg1, (byte[]) msg.obj);
            }
        }

        private void pause() {
            AudioRecorder recorder = playerReference.get();
            if (recorder != null) {
                recorder.handlePause();
            }
        }

        private void resume() {
            AudioRecorder recorder = playerReference.get();
            if (recorder != null) {
                recorder.handleResume();
            }
        }

        private void stop() {
            AudioRecorder recorder = playerReference.get();
            if (recorder != null) {
                recorder.handleStop();
            }
        }

        private void release() {
            AudioRecorder recorder = playerReference.get();
            if (recorder != null) {
                recorder.handleRelease();
            }
        }
    }
}
