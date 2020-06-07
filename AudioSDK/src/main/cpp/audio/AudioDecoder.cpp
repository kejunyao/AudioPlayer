/**
 * 音频解码器
 *
 * @author kejunyao
 * @since 2020年05月30日
 */

#include "AudioDecoder.h"

AudioDecoder::AudioDecoder(JavaCaller *javaCaller, PlayStatus *playStatus, Audio *audio) {
    this->javaCaller = javaCaller;
    this->playStatus = playStatus;
    this->audio = audio;
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutexSeek, NULL);
}

AudioDecoder::~AudioDecoder() {
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutexSeek);
}

void *prepareCallback(void *data) {
    AudioDecoder *decoder = (AudioDecoder *) data;
    decoder->prepare();
    pthread_exit(&decoder->threadDecode);
}

void AudioDecoder::prepareAsync() {
    pthread_create(&threadDecode, NULL, prepareCallback, this);
}

int avFormatCallback(void *ctx) {
    AudioDecoder *decoder = (AudioDecoder*) ctx;
    if (decoder->playStatus == NULL || decoder->playStatus->isExit()) {
        return AVERROR_EOF;
    }
    return 0;
}

void AudioDecoder::prepare() {
    pthread_mutex_lock(&mutex);
    av_register_all();
    avformat_network_init();
    avFormatContext = avformat_alloc_context();
    avFormatContext->interrupt_callback.callback = avFormatCallback;
    avFormatContext->interrupt_callback.opaque = this;

    if (avformat_open_input(&avFormatContext, audio->source, NULL, NULL) != 0) {
        if (javaCaller != NULL) {
            javaCaller->callJavaMethod(true, EVENT_ERROR, ERROR_AV_FORMAT_OPEN_INPUT, 0);
        }
        exit = true;
        pthread_mutex_unlock(&mutex);
        if (LOG_DEBUG) {
            LOGE("AudioDecoder#prepare, avformat_open_input failure");
        }
        return;
    }
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        if (javaCaller != NULL) {
            javaCaller->callJavaMethod(true, EVENT_ERROR, ERROR_AV_STREAM_FORMAT_NOT_FOUND, 0);
        }
        exit = true;
        pthread_mutex_unlock(&mutex);
        if (LOG_DEBUG) {
            LOGE("AudioDecoder#prepare, avformat_find_stream_info failure");
        }
        return;
    }

    AVCodecParameters *codecParameters = NULL;
    for (int i = 0, size = avFormatContext->nb_streams; i < size; i++) {
        AVStream *streams = avFormatContext->streams[i];
        AVCodecParameters *parameters = streams->codecpar;
        if (parameters->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (audio->streamIndex == -1) {
                audio->streamIndex = i;
                audio->setSampleRate(parameters->sample_rate);
                audio->duration = avFormatContext->duration;
                audio->timeBase = streams->time_base;
                if (LOG_DEBUG) {
                    LOGD("AudioDecoder#prepare, find AVCodecParameters");
                }
                codecParameters = parameters;
                break;
            }
        }
    }
    if (audio->streamIndex == -1 || codecParameters == NULL) {
        if (javaCaller != NULL) {
            javaCaller->callJavaMethod(true, EVENT_ERROR, ERROR_AV_MEDIA_TYPE_NOT_FOUND, 0);
        }
        exit = true;
        pthread_mutex_unlock(&mutex);
        if (LOG_DEBUG) {
            LOGE("AudioDecoder#prepare, find AVCodecParameters failure");
        }
        return;
    }
    AVCodec *decoder = avcodec_find_decoder(codecParameters->codec_id);
    if (!decoder) {
        if (javaCaller != NULL) {
            javaCaller->callJavaMethod(true, EVENT_ERROR, ERROR_AV_CODEC_DECODER_NOT_FOUND, 0);
        }
        if (LOG_DEBUG) {
            LOGE("AudioDecoder#prepare, avcodec_find_decoder failure");
        }
        exit = true;
        pthread_mutex_unlock(&mutex);
        return;
    }
    audio->avCodecContext = avcodec_alloc_context3(decoder);
    if (!audio->avCodecContext) {
        if (javaCaller != NULL) {
            javaCaller->callJavaMethod(true, EVENT_ERROR, ERROR_AV_CODEC_ALLOC_CONTEXT3, 0);
        }
        if (LOG_DEBUG) {
            LOGE("AudioDecoder#prepare, avcodec_alloc_context3 failure");
        }
        exit = true;
        pthread_mutex_unlock(&mutex);
        return;
    }
    if (avcodec_parameters_to_context(audio->avCodecContext, codecParameters) < 0) {
        if (javaCaller != NULL) {
            javaCaller->callJavaMethod(true, EVENT_ERROR, ERROR_AV_CODEC_PARAMETERS_CONTEXT, 0);
        }
        exit = true;
        pthread_mutex_unlock(&mutex);
        if (LOG_DEBUG) {
            LOGE("AudioDecoder#prepare, avcodec_parameters_to_context failure");
        }
        return;
    }
    if (avcodec_open2(audio->avCodecContext, decoder, 0) != 0) {
        if (javaCaller != NULL) {
            javaCaller->callJavaMethod(true, EVENT_ERROR, ERROR_AV_CODEC_OPEN2, 0);
        }
        exit = true;
        pthread_mutex_unlock(&mutex);
        if (LOG_DEBUG) {
            LOGE("AudioDecoder#prepare, avcodec_open2 failure");
        }
        return;
    }
    if (javaCaller != NULL) {
        javaCaller->callJavaMethod(true, EVENT_PREPARED, 0, 0);
    }
    if (playStatus == NULL || playStatus->isExit()) {
        exit = true;
    }
    pthread_mutex_unlock(&mutex);
    if (LOG_DEBUG) {
        LOGD("AudioDecoder#prepare， 预备完成");
    }
}

void *decodeCallback(void *data) {
    AudioDecoder *decoder = (AudioDecoder *) data;
    decoder->decode();
    pthread_exit(&decoder->threadDecode);
}

void AudioDecoder::decodeAsync() {
    pthread_create(&threadDecode, NULL, decodeCallback, this);
}

void AudioDecoder::decode() {
    if (LOG_DEBUG) {
        LOGD("AudioDecoder::decode()， 开始解码");
    }
    while (playStatus != NULL && !playStatus->isExit()) {
        if (avFormatContext == NULL) {
            break;
        }
        if (audio == NULL || audio->queue == NULL) {
            break;
        }
        if (playStatus->isSeek()) {
            continue;
        }
        if (audio->queue->size() > 40) {
            continue;
        }
        AVPacket *avPacket = av_packet_alloc();
        if(av_read_frame(avFormatContext, avPacket) == 0) {
            if (avPacket->stream_index == audio->streamIndex) {
                audio->queue->push(avPacket);
            } else {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
            }
        } else {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            while (playStatus != NULL && !playStatus->isExit()) {
                if (audio != NULL && audio->queue != NULL && audio->queue->size() > 0) {
                    continue;
                }
                playStatus->setExit(true);
                break;
            }
        }
    }
    exit = true;
    if (playStatus != NULL && !playStatus->isExit()) {
        if (javaCaller != NULL) {
            javaCaller->callJavaMethod(true, EVENT_COMPLETE, 0, 0);
        }
    }
    if (LOG_DEBUG) {
        LOGD("AudioDecoder#decode，解码完成。");
    }
}

void AudioDecoder::release() {
    pthread_mutex_lock(&mutex);
    int sleepCount = 0;
    while (!exit) {
        if(sleepCount > 1000) {
            exit = true;
        }
        if(LOG_DEBUG) {
            LOGE("wait ffmpeg  exit %d", sleepCount);
        }
        sleepCount++;
        av_usleep(1000 * 10);//暂停10毫秒
    }
    if (avFormatContext != NULL) {
        avformat_close_input(&avFormatContext);
        avformat_free_context(avFormatContext);
        av_free(avFormatContext);
        avFormatContext = NULL;
    }
    if (playStatus != NULL) {
        playStatus = NULL;
    }
    if (javaCaller != NULL) {
        javaCaller = NULL;
    }
    if (audio != NULL) {
        audio = NULL;
    }
    pthread_mutex_unlock(&mutex);
}

void AudioDecoder::seekByPercent(float percent) {
    if (audio == NULL) {
        return;
    }
    seek(percent * audio->durationInSecond());
}

void AudioDecoder::seek(int64_t second) {
    if (audio == NULL) {
        return;
    }
    if (playStatus == NULL) {
        return;
    }
    if (avFormatContext == NULL) {
        return;
    }
    if (second < 0 && second > audio->duration) {
        return;
    }
    playStatus->setSeek(true);
    audio->queue->clear();
    audio->reset();
    pthread_mutex_lock(&mutexSeek);
    int64_t rel = second * AV_TIME_BASE;
    avformat_seek_file(avFormatContext, -1, INT64_MIN, rel, INT64_MAX, 0);
    pthread_mutex_unlock(&mutexSeek);
    playStatus->setSeek(false);
}




