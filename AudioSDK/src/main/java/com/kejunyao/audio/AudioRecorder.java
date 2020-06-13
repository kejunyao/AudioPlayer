package com.kejunyao.audio;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import java.io.File;
import java.io.FileOutputStream;
import java.nio.ByteBuffer;

/**
 * 音频录制器
 *
 * @author kejunyao
 * @since 2020年05月30日
 */
final class AudioRecorder {

    private MediaFormat encoderFormat;
    private MediaCodec encoder;
    private FileOutputStream outputStream;
    private MediaCodec.BufferInfo info;
    private int perPCMSize = 0;
    private byte[] outByteBuffer;
    private int aacSampleRate = 4;
    private boolean sInit;

    private final AudioPlayer mPlayer;

    public AudioRecorder(AudioPlayer player) {
        mPlayer = player;
    }

    private void init(int sampleRate, File outfile) {
        try {
            aacSampleRate = getADTSSampleRate(sampleRate);
            encoderFormat = MediaFormat.createAudioFormat(MediaFormat.MIMETYPE_AUDIO_AAC, sampleRate, 2);
            encoderFormat.setInteger(MediaFormat.KEY_BIT_RATE, 96000);
            encoderFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);
            encoderFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 4096);
            encoder = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_AUDIO_AAC);
            info = new MediaCodec.BufferInfo();
            if (encoder == null) {
                return;
            }
            encoder.configure(encoderFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
            outputStream = new FileOutputStream(outfile);
            encoder.start();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void encodecPcmToAAc(int size, byte[] buffer) {
        AudioLog.d("AudioRecorder", "encodecPcmToAAc, size: ", size, ", buffer.length: ", buffer.length);
        if (buffer != null && encoder != null) {
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

                    addADtsHeader(outByteBuffer, perPCMSize, aacSampleRate);

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

    private void addADtsHeader(byte[] packet, int packetLen, int sampleRate) {
        int profile = 2; // AAC LC
        int freqIdx = sampleRate; // sampleRate
        int chanCfg = 2; // CPE

        packet[0] = (byte) 0xFF; // 0xFFF(12bit) 这里只取了8位，所以还差4位放到下一个里面
        packet[1] = (byte) 0xF9; // 第一个t位放F
        packet[2] = (byte) (((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2));
        packet[3] = (byte) (((chanCfg & 3) << 6) + (packetLen >> 11));
        packet[4] = (byte) ((packetLen & 0x7FF) >> 3);
        packet[5] = (byte) (((packetLen & 7) << 5) + 0x1F);
        packet[6] = (byte) 0xFC;
    }

    private int getADTSSampleRate(int sampleRate) {
        int rate = 4;
        switch (sampleRate) {
            case 96000:
                rate = 0;
                break;
            case 88200:
                rate = 1;
                break;
            case 64000:
                rate = 2;
                break;
            case 48000:
                rate = 3;
                break;
            case 44100:
                rate = 4;
                break;
            case 32000:
                rate = 5;
                break;
            case 24000:
                rate = 6;
                break;
            case 22050:
                rate = 7;
                break;
            case 16000:
                rate = 8;
                break;
            case 12000:
                rate = 9;
                break;
            case 11025:
                rate = 10;
                break;
            case 8000:
                rate = 11;
                break;
            case 7350:
                rate = 12;
                break;
        }
        return rate;
    }

    private void release() {
        if (encoder == null) {
            return;
        }
        try {
            outputStream.close();
            outputStream = null;
            encoder.stop();
            encoder.release();
            encoder = null;
            encoderFormat = null;
            info = null;
            sInit = false;
            AudioLog.d("AudioRecorder", "录制完成。");
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (outputStream != null) {
                try {
                    outputStream.close();
                } catch (Exception e) {
                    e.printStackTrace();
                }
                outputStream = null;
            }
        }
    }

    private File mFile;

    void start(File file, int sampleRate) {
        if (mFile != null && mFile.getAbsolutePath().equals(file.getAbsolutePath())) {
            return;
        }
        mFile = file;
        if (!sInit) {
            if (sampleRate > 0) {
                sInit = true;
                init(sampleRate, mFile);
                mPlayer._setRecord(true);
                AudioLog.d("AudioRecorder", "开始录制, sampleRate: ", sampleRate);
                return;
            }
        }
    }

    public void stop() {
        if (sInit) {
            mPlayer._setRecord(false);
            release();
            sInit = false;
            AudioLog.d("AudioRecorder", "完成录制");
        }
    }

    public void pause() {
        mPlayer._setRecord(false);
        AudioLog.d("AudioRecorder", "暂停录制");
    }

    public void resume() {
        mPlayer._setRecord(true);
        AudioLog.d("AudioRecorder", "继续录制");
    }
}
