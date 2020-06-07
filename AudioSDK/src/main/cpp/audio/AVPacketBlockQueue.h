/**
 * 音频队列(线程阻塞)
 *
 * @author kejunyao
 * @since 2020年05月30日
 */

#ifndef AUDIOPLAYER_AVPACKETBLOCKQUEUE_H
#define AUDIOPLAYER_AVPACKETBLOCKQUEUE_H

#include <queue>
#include <pthread.h>
#include "PlayStatus.h"

extern "C" {
    #include "libavcodec/avcodec.h"
};

class AVPacketBlockQueue {
private:
    std::queue<AVPacket *> queue;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    PlayStatus *playStatus;
public:
    AVPacketBlockQueue(PlayStatus *playStatus);
    ~AVPacketBlockQueue();
    int size();
    int push(AVPacket *packet);
    int pop(AVPacket *packet);
    void clear();
};


#endif //AUDIOPLAYER_AVPACKETBLOCKQUEUE_H
