//
// JniLoggers.hpp
// libjni++
//
// Created by Thomas Micheline Mar 26, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
// Copyright © 2023 VSee Lab, Inc. All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#pragma once

#include <string>
#include <iostream>

namespace jni_pp {

typedef enum LogPriority {
    LOG_DEBUG,
    LOG_WARN,
    LOG_ERROR,
    LOG_DISABLE
} LogPriority;

class JniLogger {

public:
    virtual void debug(const std::string& msg);
    virtual void warning(const std::string& msg);
    virtual void error(const std::string& msg);

protected:
    virtual void log(LogPriority level, const std::string& msg) = 0;
    static char getLevelIndicator(const LogPriority &level) ;
};

class JniStreamLogger : public JniLogger {
public:
    explicit JniStreamLogger(std::ostream& out = std::cerr) : mOut(out) {}

protected:
    void log(LogPriority level, const std::string& msg) override;

private:
    std::ostream& mOut;

};

//
// TODO: Should support a fallback mechanism... logging will be most important when JVM isn't reachable
//
class JniJvmLogger : public JniLogger {
protected:
    void log(LogPriority level, const std::string& msg) override;
};

void log_vprint(int level, const char *fmt, va_list args);
void log_print(int level, const char *fmt, ...);

void setMinimumLogLevel(LogPriority level);
LogPriority getMinimumLogLevel();

void setLogger(std::shared_ptr<JniLogger> logger);
std::shared_ptr<JniLogger> getLogger();

#define assertm(exp, msg) assert(((void)msg, exp))

} // jnipp
