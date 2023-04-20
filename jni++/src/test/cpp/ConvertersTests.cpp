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
#include "JvmTestFixture.hpp"

using namespace jni_pp;

TEST_F(JvmTestFixture, ConvertCppToJavaTest)
{
    static const char *const TEST_STRING = "this is a test";
    jobject javaStringValue = ToJavaConverter<std::string>::convertToJava<jobject>(TEST_STRING);
    const char *cstr = ToCppConverter<const char *>::convertToCpp(javaStringValue);
    ASSERT_STREQ(TEST_STRING, cstr) << "Test string '" << TEST_STRING << "' should be equal to converted string '" << cstr << "'";

    static const short kTestShort = 1001;
    jshort javaShortValue = ToJavaConverter<short>::convertToJava<jshort>(kTestShort);
    short cshort = ToCppConverter<short>::convertToCpp(javaShortValue);
    ASSERT_EQ(kTestShort, cshort) << "Test short " << kTestShort << " should be equal to converted short " << cshort;

    static const char kTestChar = '#';
    jchar javaCharValue = ToJavaConverter<char>::convertToJava<jchar>(kTestChar);
    char cchar = ToCppConverter<char>::convertToCpp(javaCharValue);
    ASSERT_EQ(kTestChar, cchar) << "Test char " << kTestChar << " should be equal to converted char " << cchar;

    static const int kTestInt = 42;
    jint javaIntValue = ToJavaConverter<int>::convertToJava<jint>(kTestInt);
    int cint = ToCppConverter<int>::convertToCpp(javaIntValue);
    ASSERT_EQ(kTestInt, cint) << "Test int " << kTestInt << " should be equal to converted int " << cint;

    static const long kTestLong = 42'000'000'000;
    jlong javaLongValue = ToJavaConverter<long>::convertToJava<jlong>(kTestLong);
    long clong = ToCppConverter<long>::convertToCpp(javaLongValue);
    ASSERT_EQ(kTestLong, clong) << "Test long " << kTestLong << " should be equal to converted long " << clong;

    static const float kTestFloat = -42.0;
    jfloat javaFloatValue = ToJavaConverter<float>::convertToJava<jfloat>(kTestFloat);
    float cfloat = ToCppConverter<float>::convertToCpp(javaFloatValue);
    ASSERT_FLOAT_EQ(kTestFloat, cfloat) << "Test float " << kTestFloat << " should be equal to converted float " << cfloat;

    static const double kTestDouble = 42'000'000'000'000'000.0;
    jdouble javaDoubleValue = ToJavaConverter<double>::convertToJava<jdouble>(kTestDouble);
    double cdouble = ToCppConverter<double>::convertToCpp(javaDoubleValue);
    ASSERT_DOUBLE_EQ(kTestDouble, cdouble) << "Test double " << kTestDouble << " should be equal to converted double " << cdouble;
}

