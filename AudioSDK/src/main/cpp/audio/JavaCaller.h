//
// Created by kejunyao on 2020/05/31.
//

#ifndef AUDIOPLAYER_JAVACALLER_H
#define AUDIOPLAYER_JAVACALLER_H

#include <jni.h>
#include <linux/stddef.h>
#include "../AndroidLog.h"

class JavaCaller {

private:
    JavaVM *jvm = NULL;
    JNIEnv *jenv = NULL;
    jobject instance;
    jmethodID jmidPostEvent;
public:
    JavaCaller(JavaVM *javaVM, JNIEnv *env, jobject *instance);
    ~JavaCaller();

    /**
     * 调用Java方法
     * @param isWorkThread true，C++子线程; false，当前线程
     * @param event 事件类型
     * @param arg1 参数1
     * @param arg2 参数2
     */
    void callJavaMethod(bool isWorkThread, int event, int arg1, int arg2);
};


#endif //AUDIOPLAYER_JAVACALLER_H
