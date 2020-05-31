/**
 * 音频信息
 *
 * @author kejunyao
 * @since 2020年05月31日
 */

#ifndef AUDIOPLAYER_AUDIO_H
#define AUDIOPLAYER_AUDIO_H
#include "AVPacketBlockQueue.h"

class Audio {
    public:
        AVCodecContext *avCodecContext = NULL;
        const char* source = NULL;
        int streamIndex = -1;
        AVPacketBlockQueue *queue = NULL;
        uint8_t *buffer = NULL;
        int dataSize = 0;
        int sampleRate = 0;
    public:
        Audio(const char* source, PlayStatus *playStatus);
        ~Audio();
        void setSampleRate(int sampleRate);
};


#endif //AUDIOPLAYER_AUDIO_H
