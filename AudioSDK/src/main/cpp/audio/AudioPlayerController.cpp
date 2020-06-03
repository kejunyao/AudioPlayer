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
}

AudioPlayerController::~AudioPlayerController() {
    if (audioOutput != NULL) {
        free(audioOutput);
        audioOutput = NULL;
    }
    if (audioDecoder != NULL) {
        free(audioDecoder);
        audioDecoder = NULL;
    }
    if (audio != NULL) {
        free(audio);
        audio = NULL;
    }
    if (playStatus != NULL) {
        free(playStatus);
        playStatus = NULL;
    }
}

void AudioPlayerController::prepare(const char *source) {
    if (playStatus == NULL) {
        playStatus = new PlayStatus();
    }
    playStatus->setExit(false);
    if (audio == NULL) {
        audio = new Audio(source, playStatus);
        audioDecoder = new AudioDecoder(javaCaller, playStatus, audio);
    }
    audioDecoder->prepareAsync();
}

void AudioPlayerController::start() {
    if (audio == NULL) {
        return;
    }
    if (audioOutput == NULL) {
        audioOutput = new AudioOutput(javaCaller, playStatus, audio);
    }
    audioOutput->play();
    audioDecoder->decodeAsync();
}

void AudioPlayerController::resume() {
    if (audioOutput == NULL) {
        return;
    }
    audioOutput->resume();
}

void AudioPlayerController::pause() {
    if (audioOutput == NULL) {
        return;
    }
    audioOutput->pause();
}

void AudioPlayerController::stop() {
    if (audioOutput == NULL) {
        return;
    }
    audioOutput->stop();
}

void AudioPlayerController::release() {
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
}
