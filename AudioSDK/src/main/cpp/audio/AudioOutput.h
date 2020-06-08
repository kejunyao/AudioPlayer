/**
 * 音频输出模块
 *
 * @author kejunyao
 * @since 2020年05月30日
 */

#ifndef AUDIOPLAYER_AUDIOOUTPUT_H
#define AUDIOPLAYER_AUDIOOUTPUT_H

#include "Audio.h"
#include "PlayStatus.h"
#include "JavaCaller.h"
#include "../AndroidLog.h"
#include "EventCode.h"
#include "ErrorCode.h"
#include "SoundTouch.h"

using namespace soundtouch;

extern "C" {
    #include "libavcodec/avcodec.h"
    #include <libswresample/swresample.h>
    #include <SLES/OpenSLES.h>
    #include <SLES/OpenSLES_Android.h>
};

class AudioOutput {
private:
    // 引擎接口
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine = NULL;

    // 混音器
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

    // PCM
    SLObjectItf pcmPlayerObject = NULL;
    SLPlayItf pcmPlayerPlay = NULL;
    // 播放音量
    SLVolumeItf pcmVolumePlay = NULL;
    float volumePercent = 1.0;
    // 音道
    SLMuteSoloItf  pcmMutePlay = NULL;
    int mute = 2;

private:
    // SoundTouch
    SoundTouch *soundTouch = NULL;
    SAMPLETYPE *sampleBuffer = NULL;
    uint8_t *outBuffer = NULL;
    float pitch = 1.0f;
    float speed = 1.0f;
    bool finished = true;
    int nb = 0;
    int num = 0;
    int dataSize = 0;

public:
    JavaCaller *javaCaller = NULL;
    PlayStatus *playStatus = NULL;
    Audio *audio = NULL;
    pthread_t threadPlay;
    // 缓冲器队列接口
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;

private:
    int getCurrentSampleRateForOpenSLES(int sampleRate);
    void checkChannels(AVFrame *frame);
    void initSampleBuffer();

public:
    AudioOutput(JavaCaller *javaCaller, PlayStatus *playStatus, Audio *audio);

    ~AudioOutput();
    void initOpenSLES();
    int resample(void **pcmBuffer);

    void play();

    /**
     * 设置音量
     * @param percent 0.0 ~ 1.0
     */
    void setVolume(float percent);

    /**
     * 设置音道
     * @param mute 0，右声道；1，左声道；2，立体声；其他值，立体声
     */
    void setMute(int mute);

    int soundTouchResample();

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

    void pause();

    void resume();

    void stop();

    void release();
};


#endif //AUDIOPLAYER_AUDIOOUTPUT_H
