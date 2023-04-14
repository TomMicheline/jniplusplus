//
// matchers.cpp
// jni++
//
// Created by Thomas Micheline Apr 10, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#include <iostream>
#include "JniPlusPlus.hpp"
#include "jni.h"

// Generate C language ABI for all functions defined in matchers.h
extern "C" {
    #include "matchers.h"
}

using namespace jni_pp;

bool setupJava(const char *jarPath) {
    setMinimumLogLevel(jni_pp::LOG_WARN);
    getLogger()->debug(std::string("Creating JVM with classpath = '") + jarPath + "'");
    auto worked = createVM(JNI_VERSION_10, jarPath);
    if (!worked) {
        std::cerr << "Failed to create Java VM!  Path=" << std::endl;
        return false;
    }
    return true;
}

jobject javaCompilePattern(const char *regex) {
    static StaticMethod<jglobal, const std::string &> jCompilePattern("java.util.regex.Pattern", "compile");
    return jCompilePattern(regex);
}

bool javaPatternMatches(jobject pattern, const char *testString) {
    static InstanceMethod<jlocal, const char *> jPatternMatcher("java.util.regex.Pattern", "matcher");
    static InstanceMethod<bool> jMatcherMatches("java.util.regex.Matcher", "matches");
    JniLocalReferenceScope refs;
    return jMatcherMatches(jPatternMatcher(pattern, testString));
}
