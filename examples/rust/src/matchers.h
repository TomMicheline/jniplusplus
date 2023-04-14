//
// matchers.h
// jni++
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

bool setupJava(const char *jarPath);
jobject javaCompilePattern(const char *regex);
bool javaPatternMatches(jobject pattern, const char *testString);
