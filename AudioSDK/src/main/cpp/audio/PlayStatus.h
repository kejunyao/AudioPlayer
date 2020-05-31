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
    pthread_cond_t cond;
    bool exit = false;
    bool load = true;
    public:
        PlayStatus();
        ~PlayStatus();
        bool isExit();
        bool isLoad();
        void setExit(bool exit);
        void setLoad(bool load);
};


#endif //AUDIOPLAYER_PLAYSTATUS_H
