/**
 * 音频播放Player
 *
 * @author kejunyao
 * @since 2020年05月30日
 */
#include <jni.h>
#include <string>
#include "AudioPlayerController.h"

JavaVM *javaVM = NULL;

extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
    javaVM = jvm;
    JNIEnv *env;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_4;
}

AudioPlayerController *playerController = NULL;

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_kejunyao_audio_AudioPlayer_isPlaying(JNIEnv *env, jobject thiz) {
    return false;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_kejunyao_audio_AudioPlayer__1start(JNIEnv *env, jobject thiz) {
    if (playerController == NULL) {
        return;
    }
    playerController->start();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_kejunyao_audio_AudioPlayer__1pause(JNIEnv *env, jobject thiz) {
    if (playerController == NULL) {
        return;
    }
    playerController->pause();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_kejunyao_audio_AudioPlayer__1stop(JNIEnv *env, jobject thiz) {
    if (playerController == NULL) {
        return;
    }
    playerController->stop();
}

pthread_t threadRelease;

void *releaseCallback(void *data) {
    if (playerController != NULL) {
        playerController->release();
        delete(playerController);
        playerController = NULL;
    }
    pthread_exit(&threadRelease);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_kejunyao_audio_AudioPlayer__1release(JNIEnv *env, jobject thiz) {
    if (playerController == NULL) {
        return;
    }
    pthread_create(&threadRelease, NULL, releaseCallback, NULL);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_kejunyao_audio_AudioPlayer__1prepare(JNIEnv *env, jobject thiz, jstring _source) {
    const char *source = env->GetStringUTFChars(_source, 0);
    if (playerController == NULL) {
        playerController = new AudioPlayerController(javaVM, env, &thiz);
    }
    playerController -> prepare(source);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_kejunyao_audio_AudioPlayer__1resume(JNIEnv *env, jobject thiz) {
    if (playerController == NULL) {
        return;
    }
    playerController->resume();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_kejunyao_audio_AudioPlayer__1seekByPercent(JNIEnv *env, jobject thiz, jfloat percent) {
    if (playerController == NULL) {
        return;
    }
    playerController->seekByPercent(percent);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_kejunyao_audio_AudioPlayer__1seek(JNIEnv *env, jobject thiz, jint second) {
    if (playerController == NULL) {
        return;
    }
    playerController->seek(second);
}