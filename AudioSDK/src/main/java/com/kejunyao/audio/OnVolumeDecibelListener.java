package com.kejunyao.audio;

/**
 * 分贝回调Listener
 *
 * @author kejunyao
 * @since 2020年05月31日
 */
public interface OnVolumeDecibelListener {
    /**
     * 分贝回调
     * @param decibel 分贝
     */
    void onVolumeDecibel(int decibel);
}
