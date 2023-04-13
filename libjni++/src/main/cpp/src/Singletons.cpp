//
// JniPPSingletons.cpp
// libjni++
//
// Created by Thomas Micheline Apr 12, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#include "jnipp/Singletons.hpp"
#include "jnipp/Utilities.hpp"

#include <mutex>
#include <map>

namespace jni_pp {

std::mutex singleton_mutex;
std::map<std::string, jobject> singleton_map;

jobject getSingletonObject(const std::string &className) {
    std::unique_lock<std::mutex> lock(singleton_mutex);
    auto it = singleton_map.find(className);
    if (it == singleton_map.end()) {
        return nullptr;
    }
    return it->second;
}

void clearSingletonObjects() {
    std::unique_lock<std::mutex> lock(singleton_mutex);
    for (auto &pair: singleton_map) {
        env()->DeleteGlobalRef(pair.second);
    }
    singleton_map.clear();
}

void registerSingleton(const std::string &className, jobject obj) {
    std::unique_lock<std::mutex> lock(singleton_mutex);
    auto it = singleton_map.find(className);
    // Only remove the singleton if both the name and object match
    if (it != singleton_map.end()) {
        if (env()->IsSameObject(it->second, obj)) {
            // already mapped
            return;
        } else {
            // Free old reference
            env()->DeleteGlobalRef(it->second);
        }
    }
    singleton_map[className] = env()->NewGlobalRef(obj);
}

void unregisterSingleton(const std::string &className, jobject obj) {
    std::unique_lock<std::mutex> lock(singleton_mutex);
    auto it = singleton_map.find(className);
    // Only remove the singleton if both the name and object match
    if (it != singleton_map.end() && env()->IsSameObject(it->second, obj)) {
        env()->DeleteGlobalRef(it->second);
        singleton_map.erase(it);
    }
}

}