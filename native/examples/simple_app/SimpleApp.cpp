//
// SimpleApp.cpp
// libjni++
//
// Created by Thomas Micheline Mar 27, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
// Copyright © 2023 VSee Lab, Inc. All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//


#include "JniPlusPlus.hpp"

#include <jni.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>

using namespace jni_pp;

int main(int argc, const char **argv)
{
    setMinimumLogLevel(jni_pp::LOG_WARN);

    std::string path("../../../../java/main/build/libs/libjni++.jar");

    if (argc > 1) {
        path = argv[1];
    }

    std::ifstream f(path.c_str());
    if (!f.good()) {
        std::cerr << "Error: Can't find jar file '" << path << "'.  You can specify the full path of the libjni++.jar file as the first command line argument" << std::endl;
        return -1;
    }

    auto worked = createVM(JNI_VERSION_10, path);
    if (!worked) {
        std::cerr << "Error: Failed to create Java VM!" << std::endl;
        return -1;
    }

    //
    // Example 1) Create a java string and then create an all-caps version of it.
    //
    Constructor<jlocal, const std::string&> jStringCtor("java.lang.String", "(Ljava/lang/String;)V");
    InstanceMethod<std::string> jStringToUpper("java.lang.String", "toUpperCase");

    auto jstr = jStringCtor("hello, world.");
    std::cout << "Is this upper case? '" << jStringToUpper(jstr) << "'" << std::endl;

    //
    // Example 2) Create and use a regex pattern matcher.  This is equivalent to the following Java code:
    //
    //    Pattern pattern = Pattern.compile(".*JNI.*rocks.*");
    //    Matcher matcher1 = pattern.matcher("This strings will see if JNI is dumb as rocks or not");
    //    Matcher matcher2 = pattern.matcher("This strings will see if jni is dumb as rocks or not");
    //    System.out.printf("Does the pattern match text #1? %s%n", matcher1.matches() ? "YES" : "NO");
    //    System.out.printf("Does the pattern match text #2? %s%n", matcher2.matches() ? "YES" : "NO");
    //

    //
    // First, the plain JNI version that only uses env() from jni++
    //
    jclass patternClass = env()->FindClass("java/util/regex/Pattern");
    jclass matcherClass = env()->FindClass("java/util/regex/Matcher");
    if (!patternClass || !matcherClass) {
        getLogger()->error("SimpleApp: Could not get JNI classes for plain JNI example.");
        return -1;
    }

    jmethodID compileMethod = env()->GetStaticMethodID(patternClass, "compile", "(Ljava/lang/String;)Ljava/util/regex/Pattern;");
    jmethodID matcherMethod = env()->GetMethodID(patternClass, "matcher", "(Ljava/lang/CharSequence;)Ljava/util/regex/Matcher;");
    jmethodID matchesMethod = env()->GetMethodID(matcherClass, "matches", "()Z");
    if (!compileMethod || !matcherMethod || !matchesMethod) {
        getLogger()->error("SimpleApp: Could not get JNI methods for plain JNI example.");
        return -1;
    }

    jstring patternString = env()->NewStringUTF(".*JNI.*rocks.*");
    jobject patternObject = env()->CallStaticObjectMethod(patternClass, compileMethod, patternString);
    jstring matchingString1 = env()->NewStringUTF("This strings will see if JNI is dumb as rocks or not");
    jobject matcher1Object = env()->CallObjectMethod(patternObject, matcherMethod, matchingString1);
    jstring matchingString2 = env()->NewStringUTF("This strings will see if jni is dumb as rocks or not");
    jobject matcher2Object = env()->CallObjectMethod(patternObject, matcherMethod, matchingString2);

    jboolean result1 = env()->CallBooleanMethod(matcher1Object, matchesMethod);
    jboolean result2 = env()->CallBooleanMethod(matcher2Object, matchesMethod);

    std::cout << "[Plain JNI] Does the pattern match text #1? " << (result1 ? "YES" : "NO") << std::endl;
    std::cout << "[Plain JNI] Does the pattern match text #2? " << (result2 ? "YES" : "NO") << std::endl;

    //
    // Now the jni++ version
    //

    StaticMethod<jlocal, const std::string&> jCompilePattern("java.util.regex.Pattern", "compile");
    InstanceMethod<jlocal, const std::string&> jPatternMatcher("java.util.regex.Pattern", "matcher");
    InstanceMethod<bool> jMatcherMatches("java.util.regex.Matcher", "matches");

    auto pattern = jCompilePattern(".*JNI.*rocks.*");
    auto matcher1 = jPatternMatcher(pattern, "This strings will see if JNI is dumb as rocks or not");
    auto matcher2 = jPatternMatcher(pattern, "This strings will see if jni is dumb as rocks or not");

    std::cout << "Does the pattern match text #1? " << (jMatcherMatches(matcher1) ? "YES" : "NO") << std::endl;
    std::cout << "Does the pattern match text #2? " << (jMatcherMatches(matcher2) ? "YES" : "NO") << std::endl;


    destroyVM();
}
