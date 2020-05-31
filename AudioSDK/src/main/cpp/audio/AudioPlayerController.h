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
    public:
        AudioPlayerController(JavaVM *javaVM, JNIEnv *env, jobject *instance);
        ~AudioPlayerController();
        void prepare(const char *source);
        void start();
        void resume();
        void pause();
        void stop();
        void release();
};


#endif //AUDIOPLAYER_AUDIOPLAYERCONTROLLER_H
