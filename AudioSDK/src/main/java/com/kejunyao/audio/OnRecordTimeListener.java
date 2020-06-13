package com.kejunyao.audio;

/**
 * 录音时间Listener
 *
 * @author kejunyao
 * @since 2020年06月13日
 */
public interface OnRecordTimeListener {

    /**
     * 录音时间回调方法
     * @param time 录音时间
     */
    void onRecordTime(int time);
}
