/**
 * 音频解码器
 *
 * @author kejunyao
 * @since 2020年05月30日
 */

#ifndef AUDIOPLAYER_AUDIODECODER_H
#define AUDIOPLAYER_AUDIODECODER_H

#include "AVPacketBlockQueue.h"
#include "PlayStatus.h"
#include "../AndroidLog.h"
#include "Audio.h"
#include "JavaCaller.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

class AudioDecoder {
private:
    PlayStatus *playStatus;
    Audio *audio = NULL;
    JavaCaller *javaCaller;
    AVFormatContext *avFormatContext = NULL;
    // AVCodecParameters *codecpar = NULL;
    // VPacket *avPacket = NULL;
    // AVFrame *avFrame = NULL;

public:
    pthread_t threadDecode;

public:
    AudioDecoder(JavaCaller *javaCaller, PlayStatus *playStatus, Audio *audio);

    ~AudioDecoder();

    void prepare();

    void prepareAsync();

    void decode();

    void decodeAsync();


};


#endif //AUDIOPLAYER_AUDIODECODER_H
