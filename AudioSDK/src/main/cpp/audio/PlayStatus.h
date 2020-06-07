/**
 * 播放器状态
 *
 * @author kejunyao
 * @since 2020年05月30日
 */

#ifndef AUDIOPLAYER_PLAYSTATUS_H
#define AUDIOPLAYER_PLAYSTATUS_H


#include <pthread.h>

class PlayStatus {
private:
    pthread_mutex_t mutex;
    pthread_mutex_t mutexSeek;
    bool exit = false;
    bool load = true;
    bool seek = false;
    public:
        PlayStatus();
        ~PlayStatus();
        bool isExit();
        bool isLoad();
        bool isSeek();
        void setExit(bool exit);
        void setLoad(bool load);
        void setSeek(bool seek);
};


#endif //AUDIOPLAYER_PLAYSTATUS_H
