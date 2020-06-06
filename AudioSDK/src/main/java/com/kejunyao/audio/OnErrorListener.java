package com.kejunyao.audio;

/**
 * 错误Listener
 *
 * @author kejunyao
 * @since 2020年06月06日
 */
public interface OnErrorListener {
    /**
     * 错误回调
     * @param errCode {@link ErrorCode}
     */
    void onError(int errCode);
}
