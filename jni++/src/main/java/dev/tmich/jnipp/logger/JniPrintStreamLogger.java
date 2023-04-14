//
// dev.tmich.jnipp.logger.JniPrintStreamLogger.java
// jni++//
// Created by Thomas Micheline Mar 26, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
// Copyright © 2023 VSee Lab, Inc. All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

package dev.tmich.jnipp.logger;

import java.io.PrintStream;

/**
 * Simple JniLogger that just writes to supplied PrintStream or System.out if none is passed to the
 * constructor.
 */
@SuppressWarnings("unused")
public class JniPrintStreamLogger implements JniLogger {
    private final PrintStream mStream;

    public JniPrintStreamLogger() {
        mStream = System.out;
    }
    public JniPrintStreamLogger(PrintStream stream) {
        mStream = stream;
    }

    @Override
    public void debug(String msg, Object... args) {
        if (JniLogger.shouldLog(LogLevel.Debug)) {
            mStream.printf("jni++[D]: " + msg + "%n", args);
        }
    }

    @Override
    public void warning(String msg, Object... args) {
        if (JniLogger.shouldLog(LogLevel.Warning)) {
            mStream.printf("jni++[W]: " + msg + "%n", args);
        }
    }

    @Override
    public void error(String msg, Object... args) {
        if (JniLogger.shouldLog(LogLevel.Error)) {
            mStream.printf("jni++[E]: " + msg + "%n", args);
        }
    }
}
