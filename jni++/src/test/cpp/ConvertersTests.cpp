//
// ConvertersTests.cpp
// jni++
//
// Created by Thomas Micheline Apr 12, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#include "gtest/gtest.h"
#include "jnipp/Converters.hpp"

using namespace jni_pp;

class ConvertersTestsFixture : public ::testing::Test {

protected:
protected:
    virtual void TearDown() {
        destroyVM();
    }

    virtual void SetUp() {
        // Current directory is jniplusplus/native_project/cmake-build-debug/jni++_tests
        auto worked = createVM(JNI_VERSION_10, "../../../jni++/build/libs/jni++.jar");
        if (!worked) {
            std::cerr << "Error: Failed to create Java VM!" << std::endl;
            abort();
        }
    }
};

TEST_F(ConvertersTestsFixture, ConvertCppToJavaTest)
{
    static const char *const TEST_STRING = "this is a test";
    typedef typename CppToJava<std::string>::type JavaStringType;
    jobject javaStringValue = ToJavaConverter<JavaStringType>::convertToJava(TEST_STRING);
    const char *cstr = ToCppConverter<const char *>::convertToCpp(javaStringValue);
    ASSERT_STREQ(TEST_STRING, cstr) << "Test string '" << TEST_STRING << "' should be equal to converted string '" << cstr << "'";

    static short TEST_SHORT = 1001;
    typedef typename  CppToJava<short>::type JavaShortType;
    jshort javaShortValue = ToJavaConverter<JavaShortType>::convertToJava(TEST_SHORT);
    short cshort = ToCppConverter<short>::convertToCpp(javaShortValue);
    ASSERT_EQ(TEST_SHORT, cshort) << "Test short " << TEST_SHORT << " should be equal to converted short " << cshort;

    static char TEST_CHAR = '#';
    typedef typename  CppToJava<char>::type JavaCharType;
    jchar javaCharValue = ToJavaConverter<JavaCharType>::convertToJava(TEST_CHAR);
    char cchar = ToCppConverter<char>::convertToCpp(javaCharValue);
    ASSERT_EQ(TEST_CHAR, cchar) << "Test char " << TEST_CHAR << " should be equal to converted char " << cchar;

    static int TEST_INT = 42;
    typedef typename  CppToJava<int>::type JavaIntType;
    jint javaIntValue = ToJavaConverter<JavaIntType>::convertToJava(TEST_INT);
    int cint = ToCppConverter<int>::convertToCpp(javaIntValue);
    ASSERT_EQ(TEST_INT, cint) << "Test int " << TEST_INT << " should be equal to converted int " << cint;

    static long TEST_LONG = 42'000'000'000;
    typedef typename  CppToJava<long>::type JavaLongType;
    jlong javaLongValue = ToJavaConverter<JavaLongType>::convertToJava(TEST_LONG);
    long clong = ToCppConverter<long>::convertToCpp(javaLongValue);
    ASSERT_EQ(TEST_LONG, clong) << "Test long " << TEST_LONG << " should be equal to converted long " << clong;

    static float TEST_FLOAT = -42.0;
    typedef typename  CppToJava<float>::type JavaFloatType;
    jfloat javaFloatValue = ToJavaConverter<JavaFloatType>::convertToJava(TEST_FLOAT);
    float cfloat = ToCppConverter<float>::convertToCpp(javaFloatValue);
    ASSERT_EQ(TEST_FLOAT, cfloat) << "Test float " << TEST_FLOAT << " should be equal to converted float " << cfloat;

    static double TEST_DOUBLE = 42'000'000'000'000'000.0;
    typedef typename  CppToJava<double>::type JavaDoubleType;
    jdouble javaDoubleValue = ToJavaConverter<JavaDoubleType>::convertToJava(TEST_DOUBLE);
    double cdouble = ToCppConverter<double>::convertToCpp(javaDoubleValue);
    ASSERT_EQ(TEST_DOUBLE, cdouble) << "Test double " << TEST_DOUBLE << " should be equal to converted double " << cdouble;
}

