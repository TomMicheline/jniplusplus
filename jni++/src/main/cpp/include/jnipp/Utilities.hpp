//
// JniUtilities.hpp
// jni++
//
// Created by Thomas Micheline Jun 22, 2013.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
// Copyright © 2023 VSee Lab, Inc. All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#pragma once

#include <jni.h>
#include <string>
#include <vector>
#include <set>
#include <cassert>
#include <condition_variable>

#include "jnipp/Exceptions.hpp"
#include "jnipp/Loggers.hpp"


namespace jni_pp {

template<typename T> using set =    std::set<T>;
using ::std::vector;

// Internal use only
void initializeEnvironment();

// For managing startup and shutdown of VM from native code.  Not relevant for managed platforms like Android
bool createVM(jint version, const std::string& classPath);
void destroyVM();

bool isEnvSetup();
JNIEnv *env();
void setEnv(JNIEnv *env);
JavaVM *getVM();
void setVM(JavaVM *vm);

//
// Set or get the current (default) JVM version.  For shared libraries loaded from the jvm this is the minimum
// version and should be set before JNI_OnLoad is called (probably from a static context).  For statically linked
// applications, this is set by createVM and then returned from JNI_OnLoad_jniplusplus and doesn't need to be set
// separately.
//
void setCurrentVersion(jint version);
jint getCurrentVersion();

bool attachCurrentThread();
void detachCurrentThread();

typedef struct MethodInfo {
    ~MethodInfo() {
        try {
            env()->DeleteGlobalRef(class_);
        } catch (...) {} // Best effort, ignore errors
    }

    jclass class_;
    jmethodID methodID;
} MethodInfo;

typedef struct FieldInfo {
    ~FieldInfo() {
        try {
            env()->DeleteGlobalRef(class_);
        } catch (...) {} // Best effort, ignore errors
    }

    jclass class_;
    jfieldID fieldID;
} FieldInfo;

jclass getClass(const std::string &name);
MethodInfo *getMethodInfo(const std::string& className, const std::string& methodName, const std::string& signature, bool isStatic, int numParameters);
FieldInfo *getFieldInfo(const std::string& className, const std::string& fieldName, const std::string& signature, bool isStatic);

void setPackageBase(const std::string& packageName);
const std::string& getPackageBase();
void setSwigPackage(const std::string& packageName);
const std::string& getSwigPackage();

inline std::string JvmClassNameToJniSignature(const std::string& className, bool addElAndSemi = true) {
    std::string classDescriptor;
    std::replace_copy(className.begin(), className.end(), std::back_inserter<std::string>(classDescriptor), '.', '/');

    if (addElAndSemi) {
        classDescriptor = std::string("L") + classDescriptor + ";";
    }

    return classDescriptor;
}

} // namespace jni_pp
