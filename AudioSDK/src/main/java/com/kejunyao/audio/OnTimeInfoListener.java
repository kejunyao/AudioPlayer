package com.kejunyao.audio;

/**
 * 播放时间Listener
 *
 * @author kejunyao
 * @since 2020年05月31日
 */
public interface OnTimeInfoListener {
    /**
     * 播放时间回调
     * @param currentTime 当前时间(单位：秒)
     * @param totalTime 总的时间(单位：秒)
     */
    void onTimeInfo(int currentTime, int totalTime);
}
