package dev.tmich.jnipp.android;

import android.util.Log;

import dev.tmich.jnipp.logger.JniLogger;

public class AndroidLogger implements JniLogger {
    @Override
    public void debug(String msg, Object... args) {
        Log.d("jni++", String.format(msg, args));
    }

    @Override
    public void warning(String msg, Object... args) {
        Log.w("jni++", String.format(msg, args));
    }

    @Override
    public void error(String msg, Object... args) {
        Log.e("jni++", String.format(msg, args));
    }
}
