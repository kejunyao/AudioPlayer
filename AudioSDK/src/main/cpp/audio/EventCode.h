/**
 * 事件标识码
 *
 * @author kejunyao
 * @since 2020年05月30日
 */

#ifndef AUDIOPLAYER_EVENTCODE_H
#define AUDIOPLAYER_EVENTCODE_H

#define EVENT_NOP        0
#define EVENT_PREPARED   1
#define EVENT_LOADING    2
#define EVENT_TIME_INFO  5 // why is 5, PAUSE = 3, RESUME = 4
#define EVENT_ERROR      6
#define EVENT_COMPLETE   7

#endif //AUDIOPLAYER_EVENTCODE_H
