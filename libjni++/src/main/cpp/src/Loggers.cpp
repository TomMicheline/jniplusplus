//
// JniLoggers.cpp
// libjni++
//
// Created by Thomas Micheline Mar 26, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
// Copyright © 2023 VSee Lab, Inc. All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#include "jnipp/Loggers.hpp"
#include "JniPlusPlus.hpp"

#include <iostream>

namespace jni_pp {

std::shared_ptr<JniLogger> gLog = std::make_shared<JniStreamLogger>();
LogPriority gMinimumLogLevel = LOG_DEBUG;

void setJavaMinimumLogLevel() {
    if (isEnvSetup()) {
        static StaticMethod<void, int> jSetMinimumLogLevel("dev.tmich.jnipp.JavaToNativeExporter", "setMinimumLogLevel");
        jSetMinimumLogLevel(gMinimumLogLevel);
    }
}

void setMinimumLogLevel(LogPriority level) {
    gMinimumLogLevel = level;
    setJavaMinimumLogLevel();
}

LogPriority getMinimumLogLevel() {
    return gMinimumLogLevel;
}

void setLogger(std::shared_ptr<JniLogger> logger)
{
    gLog = std::move(logger);
}

std::shared_ptr<JniLogger> getLogger()
{
    return gLog;
}

void log_vprint(int level, const char *fmt, va_list args) {
    char dest[1024];
    vsnprintf(dest, 1024, fmt, args);
    switch (level) {
        default:
        case LOG_DEBUG:
            gLog->debug(dest);
            break;
        case LOG_WARN:
            gLog->warning(dest);
            break;
        case LOG_ERROR:
            gLog->error(dest);
            break;
    }
}

void log_print(int level, const char *fmt, ...) {
    va_list argptr;
    va_start(argptr, fmt);
    log_vprint(level, fmt, argptr);
    va_end(argptr);
}

char JniLogger::getLevelIndicator(const LogPriority &level) {
    switch (level) {
        case LOG_DEBUG:   return 'D';
        case LOG_WARN:    return 'W';
        case LOG_ERROR:   return 'E';
        default: return 'X';
    }
}

void JniLogger::debug(const std::string &msg) {
    if (getMinimumLogLevel() <= LOG_DEBUG) {
        log(LOG_DEBUG, msg);
    }
}

void JniLogger::warning(const std::string &msg) {
    if (getMinimumLogLevel() <= LOG_WARN) {
        log(LOG_WARN, msg);
    }
}

void JniLogger::error(const std::string &msg) {
    if (getMinimumLogLevel() <= LOG_ERROR) {
        log(LOG_ERROR, msg);
    }
}

void JniStreamLogger::log(LogPriority level, const std::string &msg) {
    mOut << "libjni++ [" << getLevelIndicator(level) << "]: " << msg << std::endl;
}

StaticMethod<void, int, const std::string&> jJvmLog("dev.tmich.jnipp.JavaToNativeExporter", "jvmLog");

void JniJvmLogger::log(LogPriority level, const std::string &msg) {
    jJvmLog(level, msg);
}


} // jnipp