//
// Created by kejunyao on 2020/05/31.
//

#include "JavaCaller.h"

JavaCaller::JavaCaller(JavaVM *jvm, JNIEnv *env, jobject *instance) {
    this->jvm = jvm;
    this->jenv = env;
    this->instance = env->NewGlobalRef(*instance);
    jclass clz = jenv->GetObjectClass(this->instance);
    if (!clz) {
        if (LOG_DEBUG) {
            LOGE("can not get instance class");
        }
        return;
    }
    jmidPostEvent = jenv->GetMethodID(clz, "postEventFromNative", "(III)V");
    jmidEncodecPcmToAAc = jenv->GetMethodID(clz, "encodecPcmToAAc", "(I[B)V");
}

JavaCaller::~JavaCaller() {
}

void JavaCaller::callJavaMethod(bool isWorkThread, int event, int arg1, int arg2) {
    if (isWorkThread) {
        JNIEnv *env;
        if (jvm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("Attach current thread failure.");
            }
        }
        env->CallVoidMethod(instance, jmidPostEvent, event, arg1, arg2);
        jvm->DetachCurrentThread();
        return;
    }
    jenv->CallVoidMethod(instance, jmidPostEvent, event, arg1, arg2);
}

void JavaCaller::encodecPcmToAAc(bool isWorkThread, int size, void *buffer) {
    if (isWorkThread) {
        JNIEnv *env;
        if (jvm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("Attach current thread failure.");
            }
        }
        jbyteArray jbuffer = env->NewByteArray(size);
        env->SetByteArrayRegion(jbuffer, 0, size, static_cast<const jbyte *>(buffer));
        env->CallVoidMethod(instance, jmidEncodecPcmToAAc, size, jbuffer);
        env->DeleteLocalRef(jbuffer);
        jvm->DetachCurrentThread();
        return;
    }
    jbyteArray jbuffer = jenv->NewByteArray(size);
    jenv->SetByteArrayRegion(jbuffer, 0, size, static_cast<const jbyte *>(buffer));
    jenv->CallVoidMethod(instance, jmidEncodecPcmToAAc, size, jbuffer);
    jenv->DeleteLocalRef(jbuffer);
}


