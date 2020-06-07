/**
 * 音频输出模块
 *
 * @author kejunyao
 * @since 2020年05月30日
 */

#include "AudioOutput.h"

AudioOutput::AudioOutput(JavaCaller *javaCaller, PlayStatus *playStatus, Audio *audio) {
    this->javaCaller = javaCaller;
    this->playStatus = playStatus;
    this->audio = audio;
}

AudioOutput::~AudioOutput() {
}

void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void *context) {
    AudioOutput *output = (AudioOutput *) context;
    if (output != NULL) {
        int bufferSize = output->resample();
        if (bufferSize > 0) {
            output->audio->updateClock();
            if (output->audio->shouldRefresh()) {
                output->javaCaller->callJavaMethod(
                        true,
                        EVENT_TIME_INFO,
                        output->audio->clock,
                        output->audio->durationInSecond()
                        );
            }
            (*output->pcmBufferQueue)->Enqueue(
                    output->pcmBufferQueue,
                    (char *) output->audio->buffer,
                    bufferSize
            );
        }
    }
}

void AudioOutput::initOpenSLES() {
    SLresult result;
    result = slCreateEngine(&engineObject, 0, 0, 0, 0, 0);
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);

    // 第二步，创建混音器
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, mids, mreq);
    (void) result;
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    (void) result;
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        (void) result;
    }
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&outputMix, 0};


    // 第三步，配置PCM格式信息
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
            2
    };

    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM, // 播放pcm格式的数据
            2, // 2个声道（立体声）
            getCurrentSampleRateForOpenSLES(audio->sampleRate), // 44100hz的频率
            SL_PCMSAMPLEFORMAT_FIXED_16,// 位数 16位
            SL_PCMSAMPLEFORMAT_FIXED_16,// 和位数一致就行
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT, // 立体声（前左前右）
            SL_BYTEORDER_LITTLEENDIAN // 结束标志
    };
    SLDataSource slDataSource = {&android_queue, &pcm};

    const int LENGTH = 2;
    const SLInterfaceID ids[LENGTH] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME};
    const SLboolean req[LENGTH] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    (*engineEngine)->CreateAudioPlayer(
            engineEngine,
            &pcmPlayerObject,
            &slDataSource,
            &audioSnk,
            LENGTH,
            ids,
            req);
    // 初始化播放器
    (*pcmPlayerObject)->Realize(pcmPlayerObject, SL_BOOLEAN_FALSE);

    // 得到接口后调用  获取Player接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_PLAY, &pcmPlayerPlay);

    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_VOLUME, &pcmVolumePlay);

    // 注册回调缓冲区 获取缓冲队列接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_BUFFERQUEUE, &pcmBufferQueue);
    // 缓冲接口回调
    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallBack, this);
    // 获取播放状态接口
    (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);
    pcmBufferCallBack(pcmBufferQueue, this);

    setVolume(volumePercent);
}

int AudioOutput::getCurrentSampleRateForOpenSLES(int sampleRate) {
    int rate = 0;
    switch (sampleRate) {
        case 8000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            rate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            rate = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            rate = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate = SL_SAMPLINGRATE_44_1;
    }
    return rate;
}

void *playCallback(void *data) {
    AudioOutput *out = (AudioOutput*) data;
    out->initOpenSLES();
    if (out->threadPlay != NULL) {
        pthread_exit(&out->threadPlay);
    }
}

void AudioOutput::play() {
    pthread_create(&threadPlay, NULL, playCallback, this);
    if (LOG_DEBUG) {
        LOGD("AudioOutput#play, 开辟线程播放, source: %s", audio->source);
    }
}


void AudioOutput::setVolume(float percent) {
    if (pcmVolumePlay == NULL) {
        return;
    }
    int value = 100 * percent;
    if (value < 0 || value > 100) {
        return;
    }
    volumePercent = percent;
    if(value > 30) {
        (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - value) * -20);
        return;
    }
    if(value > 25) {
        (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - value) * -22);
        return;
    }
    if(value > 20) {
        (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - value) * -25);
        return;
    }
    if(value > 15) {
        (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - value) * -28);
        return;
    }
    if(value > 10) {
        (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - value) * -30);
        return;
    }
    if(percent > 5) {
        (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - value) * -34);
        return;
    }
    if(value > 3) {
        (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - value) * -37);
        return;
    }
    if(value > 0) {
        (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - value) * -40);
        return;
    }
    (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - value) * -100);
}

void AudioOutput::pause() {
    if (pcmPlayerPlay != NULL) {
        (*pcmPlayerPlay) -> SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PAUSED);
    }
    if (LOG_DEBUG) {
        LOGD("AudioOutput#pause, 暂停播放, source: %s", audio->source);
    }
}

void AudioOutput::resume() {
    if (pcmPlayerPlay != NULL) {
        (*pcmPlayerPlay) -> SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);
    }
    if (LOG_DEBUG) {
        LOGD("AudioOutput#resume, 播放/重新播放, source: %s", audio->source);
    }
}

void AudioOutput::stop() {
    if (pcmPlayerPlay != NULL) {
        (*pcmPlayerPlay) -> SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_STOPPED);
    }
    if (LOG_DEBUG) {
        LOGD("AudioOutput#stop, 停止播放, source: %s", audio->source);
    }
}

void AudioOutput::checkChannels(AVFrame *frame) {
    if(frame->channels && frame->channel_layout == 0) {
        frame->channel_layout = av_get_default_channel_layout(frame->channels);
    } else if(frame->channels == 0 && frame->channel_layout > 0) {
        frame->channels = av_get_channel_layout_nb_channels(frame->channel_layout);
    }
}

int AudioOutput::resample() {
    while(playStatus != NULL && !playStatus->isExit()) {
        if (audio == NULL) {
            return 0;
        }
        if(audio->queue->size() == 0) { // 加载中
            if(!playStatus->isLoad()) {
                playStatus->setLoad(true);
                javaCaller->callJavaMethod(true, EVENT_LOADING, 0, 0);
            }
            continue;
        } else{ // 加载完成
            if(playStatus->isLoad()) {
                playStatus->setLoad(false);
                javaCaller->callJavaMethod(true, EVENT_LOADING, 1, 0);
            }
        }
        AVPacket *avPacket = av_packet_alloc();
        if(audio->queue->pop(avPacket) != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
        int ret = avcodec_send_packet(audio->avCodecContext, avPacket);
        if(ret != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
        AVFrame *avFrame = av_frame_alloc();
        ret = avcodec_receive_frame(audio->avCodecContext, avFrame);
        if(ret == 0) {
            checkChannels(avFrame);
            SwrContext *swr_ctx;
            swr_ctx = swr_alloc_set_opts(
                    NULL,
                    AV_CH_LAYOUT_STEREO,
                    AV_SAMPLE_FMT_S16,
                    avFrame->sample_rate,
                    avFrame->channel_layout,
                    (AVSampleFormat) avFrame->format,
                    avFrame->sample_rate,
                    NULL, NULL
            );
            if(!swr_ctx || swr_init(swr_ctx) <0) {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
                av_frame_free(&avFrame);
                av_free(avFrame);
                avFrame = NULL;
                swr_free(&swr_ctx);
                continue;
            }

            int nb = swr_convert(
                    swr_ctx,
                    &audio->buffer,
                    avFrame->nb_samples,
                    (const uint8_t **) avFrame->data,
                    avFrame->nb_samples);

            int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            audio->dataSize = nb * out_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

            audio->nowTime = avFrame->pts * av_q2d(audio->timeBase);
            if(audio->nowTime < audio->clock) {
                audio->nowTime = audio->clock;
            }
            audio->clock = audio->nowTime;

            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            swr_free(&swr_ctx);
            break;
        } else{
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            continue;
        }
    }
    return audio->dataSize;
}

void AudioOutput::release() {
    if (pcmPlayerObject != NULL) {
        (*pcmPlayerObject) ->Destroy(pcmPlayerObject);
        pcmPlayerObject = NULL;
        pcmPlayerPlay = NULL;
        pcmBufferQueue = NULL;
    }
    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputMixEnvironmentalReverb = NULL;
    }
    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }
}





