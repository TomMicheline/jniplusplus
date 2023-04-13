//
// JniPPExceptions.hpp
// libjni++
//
// Created by Thomas Micheline Apr 12, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//


#pragma once

#include <jni.h>
#include <stdexcept>
#include <string>

namespace jni_pp {

class no_such_method : public std::runtime_error {
public:
    explicit no_such_method(const std::string &name) : runtime_error(
            std::string("No such method '") + name + "' has been cached.") {}
};

class no_such_class : public std::runtime_error {
public:
    explicit no_such_class(const std::string &name) : runtime_error(
            std::string("No such class '") + name + "' has been cached.") {}
};


struct java_exception_details {
    explicit java_exception_details(const jthrowable &jexception);

    jclass exccls, clscls, frmcls;
    jmethodID getName, getMessage, frameToString, getStackTrace, getCause;
    std::string name;
    std::string message;
    std::vector<std::string> stacktraceCauses;
    std::vector<std::vector<std::string> > stacktraceFrames;

private:
    void appendStacktrace(const jthrowable &topException);
};

class java_exception : public std::runtime_error {
public:
    explicit java_exception(const java_exception_details &details) :
            runtime_error(std::string("An exception was thrown in JAVA [ ") + details.name + ": " + details.message + " ]") {}
    java_exception(const java_exception &e) = default;
    ~java_exception() noexcept override = default;
};

void sendExceptionEvent(jthrowable jt);
void checkForExceptions();

}