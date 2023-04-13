//
// JniPPJvmNativeImpls.hpp
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


//
// Implementations of native methods defined in the libjni++ Java library
//

extern "C"
JNIEXPORT void JNICALL
Java_dev_tmich_jnipp_JavaToNativeExporter_registerSingleton(JNIEnv *env, jclass clazz, jstring jname, jobject obj);

extern "C"
JNIEXPORT void JNICALL
Java_dev_tmich_jnipp_JavaToNativeExporter_unregisterSingleton(JNIEnv *env, jclass clazz, jstring classNameJavaString, jobject obj);

extern "C"
JNIEXPORT void JNICALL
Java_dev_tmich_jnipp_JavaToNativeExporter_unregisterAllSingletons(JNIEnv *env, jclass clazz);

extern "C"
JNIEXPORT void JNICALL
Java_dev_tmich_jnipp_JavaToNativeExporter_nativeLog(JNIEnv *env, jclass clazz, jint level, jstring msg);

extern "C"
JNIEXPORT jint JNICALL
Java_dev_tmich_jnipp_JavaToNativeExporter_nativeThreadWrapper(JNIEnv *, jclass, jlong);

