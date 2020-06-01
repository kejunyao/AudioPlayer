package com.kejunyao.audio;

/**
 * 日志
 *
 * @author kejunyao
 * @since 2020年05月31日
 */
public final class AudioLog {

    private static final boolean DEBUG = true;
    private static final String TAG_PREFIX = "AudioPlayer";
    private static final String FORMAT_LOG = "%1$s_%2$s";

    private AudioLog() {
    }

    public static void d(String tag, Object... msg) {
        if (DEBUG) {
            android.util.Log.d(tag(tag), msg(msg));
        }
    }

    public static void e(String tag, Object... msg) {
        if (DEBUG) {
            android.util.Log.e(tag(tag), msg(msg));
        }
    }

    private static String tag(String tag) {
        return String.format(FORMAT_LOG, TAG_PREFIX, tag);
    }

    private static String msg(Object... msg) {
        String result = "";
        if (msg != null && msg.length > 0) {
            if (msg.length == 0) {
                if (msg[0] != null) {
                    result = msg[0].toString();
                }
            } else {
                StringBuilder sb = new StringBuilder();
                sb.ensureCapacity(80);
                for (int i = 0, size = msg.length; i < size; i++) {
                    sb.append(msg[i]);
                }
                result = sb.toString();
            }
        }
        return result;
    }
}
