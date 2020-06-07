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
    private:
        double lastClock = 0;

    public:
        AVCodecContext *avCodecContext = NULL;
        /** 音频文件来源，本地文件、服务器文件、服务器流 */
        const char* source = NULL;
        int streamIndex = -1;
        AVPacketBlockQueue *queue = NULL;
        uint8_t *buffer = NULL;
        /** 数据大小 */
        int dataSize = 0;
        /** 音频采样率 */
        int sampleRate = 0;
        /** 音频时长 */
        int64_t duration = 0;
        AVRational timeBase;
        /** 当前时间 */
        double nowTime = 0;
        double clock = 0;

    public:
        Audio(const char* source, PlayStatus *playStatus);
        ~Audio();
        void setSampleRate(int sampleRate);
        void updateClock();
        bool shouldRefresh();
        void release();

        /**
         * 时长转为秒
         */
        int durationInSecond();

        void reset();

};


#endif //AUDIOPLAYER_AUDIO_H
