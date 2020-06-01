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
}

AudioDecoder::~AudioDecoder() {
}

void *prepareCallback(void *data) {
    AudioDecoder *decoder = (AudioDecoder *) data;
    decoder->prepare();
    pthread_exit(&decoder->threadDecode);
}

void AudioDecoder::prepareAsync() {
    pthread_create(&threadDecode, NULL, prepareCallback, this);
}

void AudioDecoder::prepare() {
    av_register_all();
    avformat_network_init();
    avFormatContext = avformat_alloc_context();
    if (avformat_open_input(&avFormatContext, audio->source, NULL, NULL) != 0) {
        // TODO 向外发送错误通知
        if (LOG_DEBUG) {
            LOGE("AudioDecoder#prepare, avformat_open_input failure, source: %s", audio->source);
        }
        return;
    }
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        // TODO 向外发送错误通知
        if (LOG_DEBUG) {
            LOGE("AudioDecoder#prepare, avformat_find_stream_info failure, source: %s", audio->source);
        }
        return;
    }

    AVCodecParameters *codecParameters = NULL;
    for (int i = 0, size = avFormatContext->nb_streams; i < size; ++i) {
        AVStream *streams = avFormatContext->streams[i];
        AVCodecParameters *parameters = streams->codecpar;
        if (parameters->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (audio->streamIndex == -1) {
                audio->streamIndex = i;
                audio->setSampleRate(parameters->sample_rate);
                audio->duration = avFormatContext->duration / AV_TIME_BASE;
                audio->timeBase = streams->time_base;
                if (LOG_DEBUG) {
                    LOGD("AudioDecoder#prepare, duration: %d, timeBase: %d", audio->duration, audio->timeBase);
                }
                codecParameters = parameters;
                break;
            }
        }
    }
    if (audio->streamIndex == -1 || codecParameters == NULL) {
        // TODO 向外发送错误通知
        if (LOG_DEBUG) {
            LOGE("AudioDecoder#prepare, find AVCodecParameters failure, source: %s", audio->source);
        }
        return;
    }
    AVCodec *decoder = avcodec_find_decoder(codecParameters->codec_id);
    if (!decoder) {
        // TODO 向外发送错误通知
        if (LOG_DEBUG) {
            LOGE("AudioDecoder#prepare, avcodec_find_decoder failure, source: %s", audio->source);
        }
        return;
    }
    audio->avCodecContext = avcodec_alloc_context3(decoder);
    if (!audio->avCodecContext) {
        // TODO 向外发送错误通知
        if (LOG_DEBUG) {
            LOGE("AudioDecoder#prepare, avcodec_alloc_context3 failure, source: %s", audio->source);
        }
        return;
    }
    if (avcodec_parameters_to_context(audio->avCodecContext, codecParameters) < 0) {
        // TODO 向外发送错误通知
        if (LOG_DEBUG) {
            LOGE("AudioDecoder#prepare, avcodec_parameters_to_context failure, source: %s", audio->source);
        }
        return;
    }
    if (avcodec_open2(audio->avCodecContext, decoder, 0) != 0) {
        // TODO 向外发送错误通知
        if (LOG_DEBUG) {
            LOGE("AudioDecoder#prepare, avcodec_open2 failure, source: %s", audio->source);
        }
        return;
    }
    javaCaller->callJavaMethod(true, EVENT_PREPARED, 0, 0);
    if (LOG_DEBUG) {
        LOGD("AudioDecoder#prepare， 预备完成, source: %s", audio->source);
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
        LOGD("AudioDecoder#decode， 开始解码, source: %s", audio->source);
    }
    while (playStatus != NULL && !playStatus->isExit()) {
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
                if (audio->queue->size() > 0) {
                    continue;
                }
                playStatus->setExit(true);
                break;
            }
        }
    }
    if (LOG_DEBUG) {
        LOGD("AudioDecoder#decode， 解码完成, source: %s, 总共解码 %d 帧", audio->source, audio->queue->size());
    }
}





