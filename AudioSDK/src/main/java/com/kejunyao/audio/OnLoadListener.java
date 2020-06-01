package com.kejunyao.audio;

/**
 * 加载Listener
 *
 * @author kejunyao
 * @since 2020年05月31日
 */
public interface OnLoadListener {
    /**
     * 加载
     * @param isLoading true, 加载中；false，加载完成
     */
    void onLoad(boolean isLoading);
}
