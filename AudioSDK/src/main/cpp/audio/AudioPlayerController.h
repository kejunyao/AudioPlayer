/**
 * 播放控制器
 * 1、协调解码器解码；
 * 2、音频信息存储；
 * 3、协调输出设备重采样及播放
 *
 * @author kejunyao
 * @since 2020年05月30日
 */

#ifndef AUDIOPLAYER_AUDIOPLAYERCONTROLLER_H
#define AUDIOPLAYER_AUDIOPLAYERCONTROLLER_H


#include "Audio.h"
#include "PlayStatus.h"
#include "AudioDecoder.h"
#include "AudioOutput.h"
#include "JavaCaller.h"

class AudioPlayerController {
    private:
        Audio *audio = NULL;
        PlayStatus *playStatus = NULL;
        AudioDecoder *audioDecoder = NULL;
        AudioOutput *audioOutput = NULL;
        JavaCaller *javaCaller = NULL;
    private:
        pthread_mutex_t mutexReleasing;
        pthread_mutex_t mutexWorking;
        bool releasing = false;
        bool working = false;
    private:
        bool isReleasing();
        void setReleasing(bool releasing);
        bool isWorking();
        void setWorking(bool working);

    public:
        AudioPlayerController(JavaVM *javaVM, JNIEnv *env, jobject *instance);
        ~AudioPlayerController();
        void prepare(const char *source);
        void start();
        void seekByPercent(float percent);
        void seek(int second);
        void resume();
        void pause();
        void stop();
        void release();
        void setVolume(float percent);
        void setMute(int mute);
        /**
        * 设置音调
        * @param pitch 音调
        */
        void setPitch(float pitch);
        /**
         * 设置音速
         * @param speed 音速
         */
        void setSpeed(float speed);

        /**
         * 是否录音
         * @param shouldRecord true，录音；false，不录音
         */
        void shouldRecord(bool shouldRecord);

        int getSampleRate();
};


#endif //AUDIOPLAYER_AUDIOPLAYERCONTROLLER_H
