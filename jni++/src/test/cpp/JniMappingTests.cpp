//
// JniMappingTests.cpp
// jni++
//
// Created by Thomas Micheline Apr 20, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#include "gtest/gtest.h"
#include "jnipp/JniMapping.hpp"
#include "jnipp/SwigSupport.hpp"
#include "jnipp/JvmNativeImpls.hpp"
#include "JvmTestFixture.hpp"
#include "TestClass.hpp"

using namespace jni_pp;


extern "C"
void JNICALL Java_dev_tmich_jnipp_test_swig_TestJNI_TestStruct_1l_1set(JNIEnv *jenv, jclass jcls, jlong jarg1, jobject jarg1_, jint jarg2);
extern "C"
jint JNICALL Java_dev_tmich_jnipp_test_swig_TestJNI_TestStruct_1l_1get(JNIEnv *jenv, jclass jcls, jlong jarg1, jobject jarg1_);

void ForceSwigInclusion() {
    JNI_OnLoad(nullptr, nullptr);
    JNI_OnLoad_jniplusplus(nullptr, nullptr);
    Java_dev_tmich_jnipp_test_swig_TestJNI_TestStruct_1l_1get(nullptr, nullptr, 0, nullptr);
    Java_dev_tmich_jnipp_test_swig_TestJNI_TestStruct_1l_1set(nullptr, nullptr, 0, nullptr, 0);
}

Constructor<JvmObject<"dev.tmich.jnipp.test.TestStructEquiv">> jTSCtor("dev.tmich.jnipp.test.TestStructEquiv");
Constructor<JvmObject<"dev.tmich.jnipp.test.TestStructEquiv">, SwigMapping<TestStruct, "dev.tmich.jnipp.test.swig.TestStruct">> jTSCtorFromSwig("dev.tmich.jnipp.test.TestStructEquiv");
InstanceMethod<int> jGetTSInt("dev.tmich.jnipp.test.TestStructEquiv", "getI");
InstanceMethod<void, int> jSetTSInt("dev.tmich.jnipp.test.TestStructEquiv", "setI");
InstanceMethod<long> jGetTSLong("dev.tmich.jnipp.test.TestStructEquiv", "getL");
InstanceMethod<void, long> jSetTSLong("dev.tmich.jnipp.test.TestStructEquiv", "setL");
InstanceMethod<char> jGetTSChar("dev.tmich.jnipp.test.TestStructEquiv", "getC");
InstanceMethod<void, char> jSetTSChar("dev.tmich.jnipp.test.TestStructEquiv", "setC");

typedef JniMapping<TestStruct*, "dev.tmich.jnipp.test.TestStructEquiv"> TestStructMapping;
template<>
inline TestStruct* ToCppConverter<TestStructMapping>::convertToCpp(jobject val) {
    auto* ts = new TestStruct;
    ts->i = jGetTSInt(val);
    ts->l = jGetTSLong(val);
    ts->c = jGetTSChar(val);
    return ts;
}
template<>
inline jobject ToJavaConverter<TestStructMapping>::convertToJava(TestStruct* value) {
    JniLocalReferenceScope ref;
    jobject tse = jTSCtor();
    jSetTSInt(tse, value->i);
    jSetTSLong(tse, value->l);
    jSetTSChar(tse, value->c);
    return JvmObjectPassThrough<jobject, false>::pass(tse, ref);
}

TEST_F(JvmTestFixture, TestStructEquivTest)
{
    const int kInitialIntValue = 123'456'789;
    const long kInitialLongValue = 123'456'789'123'456L;
    const char kInitialCharValue = ' ';

    TestStruct ts;
    ts.i = kInitialIntValue;
    ts.l = kInitialLongValue;
    ts.c = kInitialCharValue;

    StaticMethod<TestStructMapping, TestStructMapping> jTimesFive("dev.tmich.jnipp.test.TestStructEquiv", "timesFive");
    auto* ts5 = jTimesFive(&ts);

    ASSERT_EQ(ts.i*5, ts5->i);
    ASSERT_EQ(ts.l*5, ts5->l);
    ASSERT_EQ(char(ts.c*5), ts5->c);

    delete ts5;

    StaticMethod<void> jDoLoad("dev.tmich.jnipp.test.LoadJniPlusPlus", "DoLoad");
    jDoLoad();

    auto fromSwigTS = jTSCtorFromSwig(&ts);
}

