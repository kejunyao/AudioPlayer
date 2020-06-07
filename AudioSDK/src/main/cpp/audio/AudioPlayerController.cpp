/**
 * 播放控制器
 * 1、协调解码器解码；
 * 2、音频信息存储；
 * 3、协调输出设备重采样及播放
 *
 * @author kejunyao
 * @since 2020年05月30日
 */

#include "AudioPlayerController.h"

AudioPlayerController::AudioPlayerController(JavaVM *javaVM, JNIEnv *env, jobject *instance) {
    this->javaCaller = new JavaCaller(javaVM, env, instance);
    pthread_mutex_init(&mutexReleasing, NULL);
    pthread_mutex_init(&mutexWorking, NULL);
}

AudioPlayerController::~AudioPlayerController() {
    pthread_mutex_destroy(&mutexReleasing);
    pthread_mutex_destroy(&mutexWorking);
}

bool AudioPlayerController::isReleasing() {
    bool result;
    pthread_mutex_lock(&mutexReleasing);
    result = releasing;
    pthread_mutex_unlock(&mutexReleasing);
    return result;
}

void AudioPlayerController::setReleasing(bool isReleasing) {
    pthread_mutex_lock(&mutexReleasing);
    releasing = isReleasing;
    pthread_mutex_unlock(&mutexReleasing);
}


bool AudioPlayerController::isWorking() {
    bool result;
    pthread_mutex_lock(&mutexWorking);
    result = working;
    pthread_mutex_unlock(&mutexWorking);
    return result;
}

void AudioPlayerController::setWorking(bool working) {
    pthread_mutex_lock(&mutexWorking);
    this->working = working;
    pthread_mutex_unlock(&mutexWorking);
}

void AudioPlayerController::prepare(const char *source) {
    if (isReleasing()) {
        return;
    }
    if (isWorking()) {
        return;
    }
    setWorking(true);
    playStatus = new PlayStatus();
    playStatus->setExit(false);
    audio = new Audio(source, playStatus);
    audioDecoder = new AudioDecoder(javaCaller, playStatus, audio);
    audioOutput = new AudioOutput(javaCaller, playStatus, audio);
    audioDecoder->prepareAsync();
}

void AudioPlayerController::start() {
    if (isReleasing()) {
        return;
    }
    if (audio == NULL) {
        return;
    }
    if (audioOutput == NULL) {
        return;
    }
    audioOutput->play();
    audioDecoder->decodeAsync();
}


void AudioPlayerController::seekByPercent(float percent) {
    if (audioDecoder == NULL) {
        return;
    }
    audioDecoder->seekByPercent(percent);
}


void AudioPlayerController::seek(int second) {
    if (audioDecoder == NULL) {
        return;
    }
    audioDecoder->seek(second);
}

void AudioPlayerController::resume() {
    if (isReleasing()) {
        return;
    }
    if (audioOutput == NULL) {
        return;
    }
    audioOutput->resume();
}

void AudioPlayerController::pause() {
    if (isReleasing()) {
        return;
    }
    if (audioOutput == NULL) {
        return;
    }
    audioOutput->pause();
}

void AudioPlayerController::stop() {
    if (isReleasing()) {
        return;
    }
    if (!isWorking()) {
        return;
    }
    if (audioOutput == NULL) {
        return;
    }
    audioOutput->stop();
}

void AudioPlayerController::setVolume(float percent) {
    if (isReleasing()) {
        return;
    }
    if (!isWorking()) {
        return;
    }
    if (audioOutput == NULL) {
        return;
    }
    audioOutput->setVolume(percent);
}


void AudioPlayerController::setMute(int mute) {
    if (isReleasing()) {
        return;
    }
    if (!isWorking()) {
        return;
    }
    if (audioOutput == NULL) {
        return;
    }
    audioOutput->setMute(mute);
}

void AudioPlayerController::release() {
    if (isReleasing()) {
        return;
    }
    if (!isWorking()) {
        return;
    }
    setReleasing(true);
    playStatus->setExit(true);
    if (audioOutput != NULL) {
        audioOutput->release();
    }
    if (audioDecoder != NULL) {
        audioDecoder->release();
    }
    if (audio != NULL) {
        audio->release();
    }
    if (audioOutput != NULL) {
        delete(audioOutput);
        audioOutput = NULL;
    }
    if (audioDecoder != NULL) {
        delete(audioDecoder);
        audioDecoder = NULL;
    }
    if (audio != NULL) {
        delete(audio);
        audio = NULL;
    }
    if (playStatus != NULL) {
        delete(playStatus);
        playStatus = NULL;
    }
    setReleasing(false);
    setWorking(false);
    if (LOG_DEBUG) {
        LOGD("AudioPlayerController::release() 完成。");
    }
}



