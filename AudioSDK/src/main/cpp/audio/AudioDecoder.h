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
#include "EventCode.h"
#include "ErrorCode.h"

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/time.h>
}

class AudioDecoder {
private:
    pthread_mutex_t mutex;
    pthread_mutex_t mutexSeek;
    Audio *audio = NULL;
    JavaCaller *javaCaller;
    AVFormatContext *avFormatContext = NULL;
    bool exit = false;

public:
    PlayStatus *playStatus;
    pthread_t threadDecode;

public:
    AudioDecoder(JavaCaller *javaCaller, PlayStatus *playStatus, Audio *audio);

    ~AudioDecoder();

    void prepare();

    void prepareAsync();

    void seekByPercent(float percent);

    void seek(int64_t second);

    void decode();

    void decodeAsync();

    void release();

};


#endif //AUDIOPLAYER_AUDIODECODER_H
