//
// ThreadWrapper.cpp
// libjni++
//
// Created by Thomas Micheline Apr 12, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#include "jnipp/ThreadWrapper.hpp"
#include "JniPlusPlus.hpp"

namespace jni_pp {

static bool debugMode = false;

void setMode(bool debug) {
    debugMode = debug;
}

bool isDebugMode() {
    return debugMode;
}

uint32_t javaThreadWrapper(struct javaThreadArgs *args) {
    static StaticMethod<int, const char *, long> jBindToAppClassLoader("JavaToNativeExporter", "bindToAppClassLoader",
                                                                       "(Ljava/lang/String;J)I");

    bool doDetachThread = jni_pp::attachCurrentThread();

    //
    // Call to Java which will, in turn, call ..._nativeThreadWrapper (above).  With the Java frames from classes in our
    // application on the call stack, JNI functions to lookup classes will use the correct class loader.  *sigh*  Super painful.
    //
    uint32_t callbackResult = 0;
    const char *name = args->name;  // save name since args is freed during the bind operation...

    bool successful = false;
    std::string msg;
    try {
        callbackResult = (uint32_t) jBindToAppClassLoader(args->name, (long) args);
        successful = true;
    } catch (const std::exception &ex) {
        msg = ex.what();
    } catch (const std::string &ex) {
        msg = ex;
    } catch (...) {
        msg = "Unknown exception";
    }

    if (!successful) {
        jni_pp::log_print(LOG_DEBUG, "bindToAppClassLoader for thread %s threw exception!  Error: %s", name,
                          msg.c_str());
        doDetachThread = false;
    }

    if (doDetachThread) {
        jni_pp::detachCurrentThread();
    }

    return callbackResult;
}

}