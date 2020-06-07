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

public:
    AudioOutput(JavaCaller *javaCaller, PlayStatus *playStatus, Audio *audio);

    ~AudioOutput();
    void initOpenSLES();
    int resample();

    void play();

    void setVolume(float percent);

    void setMute(int mute);

    void pause();

    void resume();

    void stop();

    void release();
};


#endif //AUDIOPLAYER_AUDIOOUTPUT_H
