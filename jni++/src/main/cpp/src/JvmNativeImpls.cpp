//
// JniPPJvmNativeImpls.cpp
// jni++
//
// Created by Thomas Micheline Apr 12, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#include "jnipp/JvmNativeImpls.hpp"
#include "jnipp/Utilities.hpp"
#include "jnipp/Singletons.hpp"
#include "jnipp/Converters.hpp"
#include "jnipp/ThreadWrapper.hpp"

using namespace jni_pp;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "UnusedParameter"

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    log_print(LOG_DEBUG, "Native: JNI_OnLoad called.  Caching VM.");
    jni_pp::setVM(vm);
    initializeEnvironment();
    return JNI_VERSION_1_6;
}

extern "C"
void Java_dev_tmich_jnipp_JavaToNativeExporter_registerSingleton(JNIEnv *env, jclass clazz, jstring jname, jobject obj) {
    std::string className = ToCppConverter<std::string>::convertToCpp((jobject)jname);
    registerSingleton(className, obj);
}

extern "C"
void Java_dev_tmich_jnipp_JavaToNativeExporter_unregisterSingleton(JNIEnv *env, jclass clazz, jstring classNameJavaString, jobject obj) {
    std::string className = ToCppConverter<std::string>::convertToCpp((jobject)classNameJavaString);
    unregisterSingleton(className, obj);
}

extern "C"
void Java_dev_tmich_jnipp_JavaToNativeExporter_unregisterAllSingletons(JNIEnv *env, jclass clazz) {
    clearSingletonObjects();
}

extern "C"
void Java_dev_tmich_jnipp_JavaToNativeExporter_nativeLog(JNIEnv *env, jclass clazz, jint level, jstring jmsg) {
    std::string msg = ToCppConverter<std::string>::convertToCpp((jobject)jmsg);
    jni_pp::log_print(level, msg.c_str());
}

extern "C"
jint Java_dev_tmich_jnipp_JavaToNativeExporter_nativeThreadWrapper(JNIEnv *env, jclass cls, jlong ptrToJavaThreadArgs)
{
    auto *args = (struct javaThreadArgs *) ptrToJavaThreadArgs;
    auto start_routine = args->start_routine;
    void *arg = args->arg;
    const char* name = args->name;

    //
    // Better call this here, after we have attached to correct class loader than in javaThreadWrapper
    //
//    if ( args->priority != thread_PriorityNormal) {
//        // TODO: Set priority
//    }

    free(args);

    jint result = -1;

    bool caughtException = true;
    try {
        result = start_routine(arg);
        caughtException = false;
    } catch (const std::exception& ex) {
        jni_pp::log_print(LOG_DEBUG, "nativeThreadWrapper caught exception for thread %s. Returning -1!  Error: %s", name, ex.what());
    } catch (const std::string& ex) {
        jni_pp::log_print(LOG_DEBUG, "nativeThreadWrapper caught exception for thread %s. Returning -1!  Error: %s", name, ex.c_str());
    } catch (...) {
        jni_pp::log_print(LOG_DEBUG, "nativeThreadWrapper caught exception for thread %s. Returning -1!  Error: Unknown Exception", name);
    }

    assert((!caughtException || !jni_pp::isDebugMode()) && "Thread threw uncaught exception in debug mode");

    return result;
}
#pragma clang diagnostic pop
