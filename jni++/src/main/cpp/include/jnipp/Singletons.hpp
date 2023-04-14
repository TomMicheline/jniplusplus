//
// JniPPSingletons.hpp
// jni++
//
// Created by Thomas Micheline Apr 12, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//


#pragma once

#include <jni.h>
#include <string>

namespace jni_pp {

jobject getSingletonObject(const std::string &className);
void clearSingletonObjects();
void registerSingleton(const std::string &className, jobject obj);
void unregisterSingleton(const std::string &className, jobject obj);

}