//
// JvmTestFixture.hpp
// libjni++
//
// Created by Thomas Micheline Apr 19, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//


#pragma once

#include "gtest/gtest.h"
#include "jnipp/Converters.hpp"
#include "jnipp/JniMapping.hpp"
#include "jnipp/BoxedPrimatives.hpp"

static constexpr const auto kClasspath = "../../../jni++/build/classes/java/main:../../../jni++/build/classes/java/test";

class JvmTestFixture : public testing::Test {
protected:
    virtual void TearDown() {
    }

    virtual void SetUp() {
//        jni_pp::setMinimumLogLevel(jni_pp::LOG_WARN);
//        jni_pp::setPackageBase("dev.tmich.jnipp.test");

        // Current directory is jniplusplus/native_project/cmake-build-debug/jni++_tests
        if (!createVM()) {
            std::cerr << "Error: Failed to create Java VM!" << std::endl;
            abort();
        }
    }

private:
    static bool createVM() {
        static std::once_flag runOnce;
        bool worked = true;
        call_once(runOnce, [&worked] {
            // Current directory is jniplusplus/native_project/cmake-build-debug/jni++_tests
            worked = jni_pp::createVM(JNI_VERSION_10, kClasspath);
        });
        return worked;
    }
};