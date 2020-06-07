/**
 * 音频队列(线程阻塞)
 *
 * @author kejunyao
 * @since 2020年05月30日
 */

#include "AVPacketBlockQueue.h"

AVPacketBlockQueue::AVPacketBlockQueue(PlayStatus *playStatus) {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    this->playStatus = playStatus;
}

AVPacketBlockQueue::~AVPacketBlockQueue() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

/**
 * 获取AudioFrame队列大小
 * @return AudioFrame队列大小
 */
int AVPacketBlockQueue::size() {
    int size = 0;
    pthread_mutex_lock(&mutex);
    size = queue.size();
    pthread_mutex_unlock(&mutex);
    return size;
}

/**
 * AudioFrame入队
 * @param frame AudioFrame
 * @return 0，成功；非0，失败
 */
int AVPacketBlockQueue::push(AVPacket *packet) {
    pthread_mutex_lock(&mutex);
    queue.push(packet);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return 0;
}

/**
 * AudioFrame出队
 * @param frame AudioFrame
 * @return 0，成功；非0，失败
 */
int AVPacketBlockQueue::pop(AVPacket *packet) {
    pthread_mutex_lock(&mutex);
    while(playStatus != NULL && !playStatus->isExit()) {
        if(queue.size() > 0) {
            AVPacket *avPacket =  queue.front();
            if(av_packet_ref(packet, avPacket) == 0) {
                queue.pop();
            }
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            break;
        }
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

void AVPacketBlockQueue::clear() {
    pthread_cond_signal(&cond);
    pthread_mutex_lock(&mutex);
    while (!queue.empty()) {
        AVPacket *packet = queue.front();
        queue.pop();
        av_packet_free(&packet);
        av_free(packet);
        packet = NULL;
    }
    pthread_mutex_unlock(&mutex);
}
