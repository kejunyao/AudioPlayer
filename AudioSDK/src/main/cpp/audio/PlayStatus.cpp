/**
 * 播放器状态
 *
 * @author kejunyao
 * @since 2020年05月30日
 */

#include "PlayStatus.h"

PlayStatus::PlayStatus() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
}

PlayStatus::~PlayStatus() {
}

bool PlayStatus::isExit() {
    bool result;
    pthread_mutex_lock(&mutex);
    result = exit;
    pthread_mutex_unlock(&mutex);
    return result;
}

bool PlayStatus::isLoad() {
    bool result;
    pthread_mutex_lock(&mutex);
    result = load;
    pthread_mutex_unlock(&mutex);
    return result;
}

void PlayStatus::setExit(bool exit) {
    pthread_mutex_lock(&mutex);
    this->exit = exit;
    pthread_mutex_unlock(&mutex);
}

void PlayStatus::setLoad(bool load) {
    pthread_mutex_lock(&mutex);
    this->load = load;
    pthread_mutex_unlock(&mutex);
}
