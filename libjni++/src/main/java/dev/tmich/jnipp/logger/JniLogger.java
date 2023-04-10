//
// dev.tmich.jnipp.logger.JniLogger.java
// libjni++
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
 * Define an interface for a logging implementation.   This is a small library and the
 * logging was intentionally kept very simple.  There are a couple of implementations
 * included in the distribution but the intention is that when libjni++ is used in a
 * larger project, you will provide an implementation that just passes log messages to
 * whatever logging the rest of your project uses.
 * <p>
 * For convenience, there are implementation (one native and one Java) that pass the log
 * messages to the other environment.  Presumably most implementors will prefer handling
 * logging either on the native side or in the JVM, and it is very easy to do that with
 * the supplied loggers.  However, if you ship all the log messages from JVM to native
 * (with JniNativeLogger) AND ALSO use the supplied native logger to ship all log messages
 * from native to JVM (JniJvmLogger) it will produce an infinite loop.  Don't do that.
 *
 */

public interface JniLogger {
    public enum LogLevel {
        Debug,
        Warning,
        Error,
        None // Use in call to setMinimumLogLevel to disable all logging.
    }

    static boolean shouldLog(LogLevel level) {
        return JavaToNativeExporter.getgMinimumLogLevel().ordinal() <= level.ordinal();
    }

    void debug(String msg, Object... args);
    void warning(String msg, Object... args);
    void error(String msg, Object... args);
}
