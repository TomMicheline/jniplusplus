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

#include "JniLoggers.hpp"
#include "JniPlusPlus.hpp"

#include <iostream>

namespace jni_pp {

char JniLogger::getLevelIndicator(const LogPriority &level) {
    switch (level) {
        case LOG_DEBUG:   return 'D';
        case LOG_WARN:    return 'W';
        case LOG_ERROR:   return 'E';
        case LOG_DISABLE: return 'X';
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