//
// ThreadWrapper.hpp
// libjni++
//
// Created by Thomas Micheline Apr 12, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#pragma once

#include <cstdint>

namespace jni_pp {

// If true, uncaught exceptions in native code abort execution before returning to JVM
void setMode(bool debug);
bool isDebugMode();


//
// Pass all info needed to start native thread
//

struct javaThreadArgs {
    int (*start_routine)(void *);
    int priority;
    void *arg;
    const char *name;
};

//
// This is the function passed to pthread as the main thread function.  It attaches the jvm and calls a Java wrapper
// method that, in turn, calls the native wrapper function (above).
//

uint32_t javaThreadWrapper(struct javaThreadArgs *args);

}