//
// c_wrappers.cpp
// jni++
//
// Created by Thomas Micheline Apr 04, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#include <jni.h>
#include "JniPlusPlus.hpp"

using namespace jni_pp;

extern "C" bool setupJava(const char *jarPath) {
    setMinimumLogLevel(jni_pp::LOG_WARN);
    getLogger()->debug(std::string("Creating JVM with classpath = '") + jarPath + "'");
    auto worked = createVM(JNI_VERSION_10, jarPath);
    if (!worked) {
        std::cerr << "Failed to create Java VM!  Path=" << std::endl;
        return false;
    }
    return true;
}

extern "C" jobject javaCompilePattern(const char *regex) {
    static StaticMethod<jglobal, const std::string &> jCompilePattern("java.util.regex.Pattern", "compile");
    return jCompilePattern(regex);
}

extern "C" bool javaPatternMatches(jobject pattern, const char *testString) {
    static InstanceMethod<jlocal, const char *> jPatternMatcher("java.util.regex.Pattern", "matcher");
    static InstanceMethod<bool> jMatcherMatches("java.util.regex.Matcher", "matches");
    JniLocalReferenceScope refs;
    return jMatcherMatches(jPatternMatcher(pattern, testString));
}
