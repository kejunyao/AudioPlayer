/**
 * 播放器状态
 *
 * @author kejunyao
 * @since 2020年05月30日
 */

#include "PlayStatus.h"

PlayStatus::PlayStatus() {
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutexSeek, NULL);
}

PlayStatus::~PlayStatus() {
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutexSeek);
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

bool PlayStatus::isSeek() {
    bool result;
    pthread_mutex_lock(&mutexSeek);
    result = seek;
    pthread_mutex_unlock(&mutexSeek);
    return result;
}

void PlayStatus::setSeek(bool seek) {
    pthread_mutex_lock(&mutexSeek);
    this->seek = seek;
    pthread_mutex_unlock(&mutexSeek);
}
