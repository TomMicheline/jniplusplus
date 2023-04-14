//
// dev.tmich.jnipp.logger.JniNativeLogger.java
// jni++
//
// Created by Thomas Micheline Mar 26, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
// Copyright © 2023 VSee Lab, Inc. All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

package dev.tmich.jnipp.logger;

import dev.tmich.jnipp.JavaToNativeExporter;

/**
 * JniLogger implementation that just passes all log messages to the native code to be
 * handled by whatever logger implementation is configured there.  If you use this to ship
 * all the log messages from JVM to native AND ALSO use the supplied native logger to ship
 * all log messages from native to JVM (JniJvmLogger) it will produce an infinite loop.
 * Don't do that.
 */

public class JniNativeLogger implements JniLogger {
    @Override
    public void debug(String msg, Object... args) {
        if (JniLogger.shouldLog(LogLevel.Debug)) {
            JavaToNativeExporter.nativeLog(JniLogger.LogLevel.Debug.ordinal(), String.format(msg, args));
        }
    }

    @Override
    public void warning(String msg, Object... args) {
        if (JniLogger.shouldLog(LogLevel.Warning)) {
            JavaToNativeExporter.nativeLog(JniLogger.LogLevel.Warning.ordinal(), String.format(msg, args));
        }
    }

    @Override
    public void error(String msg, Object... args) {
        if (JniLogger.shouldLog(LogLevel.Error)) {
            JavaToNativeExporter.nativeLog(JniLogger.LogLevel.Error.ordinal(), String.format(msg, args));
        }
    }
}