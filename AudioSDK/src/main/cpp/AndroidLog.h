/**
 * Android C++层日志
 *
 * @author kejunyao
 * @since 2020年05月30日
 */

#ifndef MYMUSIC_ANDROIDLOG_H
#define MYMUSIC_ANDROIDLOG_H

#endif //MYMUSIC_ANDROIDLOG_H

#include <android/log.h>
#define LOG_DEBUG true
#define LOGD(FORMAT, ...) __android_log_print(ANDROID_LOG_DEBUG, "AudioPlayer", FORMAT, ##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR, "AudioPlayer", FORMAT, ##__VA_ARGS__);
