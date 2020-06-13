package com.kejunyao.audio.sample;

/**
 * $类描述$
 *
 * @author kejunyao
 * @since 2020年06月01日
 */
public final class TimeUtils {

    private TimeUtils() {
    }

    /**
     * 将当前时间和总的显示为"00:00:00 / 00:00:00"格式
     * @param currentTime 当前时间（单位：秒）
     * @param totalTime 总的时间（单位：秒）
     * @return "00:00:00 / 00:00:00"格式文本
     */
    public static String toTimeText(int currentTime, int totalTime) {
        return String.format("%1$s / %2$s", timeText(currentTime), timeText(totalTime));
    }

    public static String timeText(int second) {
        int hours = second / (60 * 60);
        int minutes = (second % (60 * 60)) / (60);
        int seconds = second % 60;
        StringBuilder sb = new StringBuilder();
        if (hours < 10) {
            sb.append('0');
        }
        sb.append(hours);
        sb.append(':');
        if (minutes < 10) {
            sb.append('0');
        }
        sb.append(minutes);
        sb.append(':');
        if (seconds < 10) {
            sb.append('0');
        }
        sb.append(seconds);
        return sb.toString();
    }
}
