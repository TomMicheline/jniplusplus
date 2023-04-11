//
// matchers.h
// libjni++
//
// Created by Thomas Micheline Apr 10, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#pragma once

#include <stdbool.h>

#include "jni.h"

// If we aren't including jni.h, provide opaque pointer typedef for jobject
//#ifndef JNI_OK
//typedef struct _jobject *jobject;
//#endif

bool setupJava(const char *jarPath);
jobject javaCompilePattern(const char *regex);
bool javaPatternMatches(jobject pattern, const char *testString);
