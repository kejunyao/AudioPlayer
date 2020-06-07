/**
 * 音频信息
 *
 * @author kejunyao
 * @since 2020年05月31日
 */

#include "Audio.h"

Audio::Audio(const char* source, PlayStatus *playStatus) {
    this->source = source;
    queue = new AVPacketBlockQueue(playStatus);
}

Audio::~Audio() {
}

void Audio::setSampleRate(int sampleRate) {
    this->sampleRate = sampleRate;
    if (buffer == NULL) {
        buffer = (uint8_t *) av_malloc(sampleRate * 2 * 2);
    }
}

void Audio::updateClock() {
    clock += dataSize / ((double) (sampleRate * 2 * 2));
}

bool Audio::shouldRefresh() {
    if (((int) (clock - lastClock) * 10) >= 1) {
        lastClock = clock;
        return true;
    }
    return false;
}

void Audio::release() {
    if (buffer != NULL) {
        free(buffer);
        buffer = NULL;
    }
    if (queue != NULL) {
        queue->clear();
        delete(queue);
        queue = NULL;
    }
    if (avCodecContext != NULL) {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        av_free(avCodecContext);
        avCodecContext = NULL;
    }
}

int Audio::durationInSecond() {
    return duration / AV_TIME_BASE;
}

void Audio::reset() {
    clock = 0;
    lastClock = 0;
}
