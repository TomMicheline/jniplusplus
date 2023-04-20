//
// PrimitivesTests.cpp
// jni++
//
// Created by Thomas Micheline Apr 19, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#include "gtest/gtest.h"
#include "jnipp/Converters.hpp"
#include "jnipp/JniMapping.hpp"
#include "jnipp/BoxedPrimatives.hpp"
#include "JvmTestFixture.hpp"

using namespace jni_pp;


TEST_F(JvmTestFixture, PrimitiviesBoxedTests) {
    Constructor<jobject> jCtor("dev.tmich.jnipp.test.TestBoxed");
    jobject instance = jCtor();

    //    private Integer i = 7_000_000;
    const int kDefaultIntValue = 7'000'000;
    InstanceField<jboxedint> jIntField("dev.tmich.jnipp.test.TestBoxed", "i");
    int intResult = jIntField.get(instance);
    ASSERT_EQ(kDefaultIntValue, intResult);
    const int kNewIntValue = 7'777'777;
    jIntField.set(instance, kNewIntValue);

    //    public Integer getInt() {return i;}
    InstanceMethod<jboxedint> jGetInt("dev.tmich.jnipp.test.TestBoxed", "getInt");
    intResult = jGetInt(instance);
    ASSERT_EQ(kNewIntValue, intResult);

    //    public void setInt(Integer i) {this.i = i;}
    InstanceMethod<void, jboxedint> jSetInt("dev.tmich.jnipp.test.TestBoxed", "setInt");
    jSetInt(instance, kDefaultIntValue);
    intResult = jGetInt(instance);
    ASSERT_EQ(kDefaultIntValue, intResult);

    //    public Integer timesInt(Integer inval) { return i * inval; }
    InstanceMethod<jboxedint, jboxedint> jTimesInt("dev.tmich.jnipp.test.TestBoxed", "timesInt");
    intResult = jTimesInt(instance, 3);
    ASSERT_EQ(kDefaultIntValue*3, intResult);
    
    //    private Long l = 7_000_000_000_000L;
    const long kDefaultLongValue = 7'000'000'000'000L;
    InstanceField<jboxedlong> jLongField("dev.tmich.jnipp.test.TestBoxed", "l");
    long longResult = jLongField.get(instance);
    ASSERT_EQ(kDefaultLongValue, longResult);
    const long kNewLongValue = 7'777'777'777'777;
    jLongField.set(instance, kNewLongValue);

    //    public Long getLong() {return l;}
    InstanceMethod<jboxedlong> jGetLong("dev.tmich.jnipp.test.TestBoxed", "getLong");
    longResult = jGetLong(instance);
    ASSERT_EQ(kNewLongValue, longResult);

    //    public void setLong(Long l) {this.l = l;}
    InstanceMethod<void, jboxedlong> jSetLong("dev.tmich.jnipp.test.TestBoxed", "setLong");
    jSetLong(instance, kDefaultLongValue);
    longResult = jGetLong(instance);
    ASSERT_EQ(kDefaultLongValue, longResult);

    //    public Long timesLong(Long inval) { return l * inval; }
    InstanceMethod<jboxedlong, jboxedlong> jTimesLong("dev.tmich.jnipp.test.TestBoxed", "timesLong");
    longResult = jTimesLong(instance, 3);
    ASSERT_EQ(kDefaultLongValue*3, longResult);

    //    private Short s = 7_000;
    const short kDefaultShortValue = 7'000;
    InstanceField<jboxedshort> jShortField("dev.tmich.jnipp.test.TestBoxed", "s");
    short shortResult = jShortField.get(instance);
    ASSERT_EQ(kDefaultShortValue, shortResult);
    const short kNewShortValue = 7'777;
    jShortField.set(instance, kNewShortValue);

    //    public Short getShort() {return s;}
    InstanceMethod<jboxedshort> jGetShort("dev.tmich.jnipp.test.TestBoxed", "getShort");
    shortResult = jGetShort(instance);
    ASSERT_EQ(kNewShortValue, shortResult);

    //    public void setShort(Short s) {this.s = s;}
    InstanceMethod<void, jboxedshort> jSetShort("dev.tmich.jnipp.test.TestBoxed", "setShort");
    jSetShort(instance, kDefaultShortValue);
    shortResult = jGetShort(instance);
    ASSERT_EQ(kDefaultShortValue, shortResult);

    //    public Short timesShort(Short inval) { return (short) (s * inval); }
    InstanceMethod<jboxedshort, jboxedshort> jTimesShort("dev.tmich.jnipp.test.TestBoxed", "timesShort");
    shortResult = jTimesShort(instance, 3);
    ASSERT_EQ(kDefaultShortValue*3, shortResult);
    
    //    private Byte b = 70;
    const unsigned char kDefaultByteValue = 70;
    InstanceField<jboxedbyte> jByteField("dev.tmich.jnipp.test.TestBoxed", "b");
    unsigned char byteResult = jByteField.get(instance);
    ASSERT_EQ(kDefaultByteValue, byteResult);
    const unsigned char kNewByteValue = 77;
    jByteField.set(instance, kNewByteValue);

    //    public Byte getByte() {return b;}
    InstanceMethod<jboxedbyte> jGetByte("dev.tmich.jnipp.test.TestBoxed", "getByte");
    byteResult = jGetByte(instance);
    ASSERT_EQ(kNewByteValue, byteResult);

    //    public void setByte(Byte b) {this.b = b;}
    InstanceMethod<void, jboxedbyte> jSetByte("dev.tmich.jnipp.test.TestBoxed", "setByte");
    jSetByte(instance, kDefaultByteValue);
    byteResult = jGetByte(instance);
    ASSERT_EQ(kDefaultByteValue, byteResult);

    //    public Byte timesByte(Byte inval) { return (byte) (b * inval); }
    InstanceMethod<jboxedbyte, jboxedbyte> jTimesByte("dev.tmich.jnipp.test.TestBoxed", "timesByte");
    byteResult = jTimesByte(instance, 3);
    ASSERT_EQ(kDefaultByteValue*3, byteResult);
    
    //    private Character c = '7';
    const char kDefaultCharValue = '7';
    InstanceField<jboxedchar> jCharField("dev.tmich.jnipp.test.TestBoxed", "c");
    char charResult = jCharField.get(instance);
    ASSERT_EQ(kDefaultCharValue, charResult);
    const char kNewCharValue = 'D';
    jCharField.set(instance, kNewCharValue);

    //    public Character getCharacter() {return c;}
    InstanceMethod<jboxedchar> jGetChar("dev.tmich.jnipp.test.TestBoxed", "getCharacter");
    charResult = jGetChar(instance);
    ASSERT_EQ(kNewCharValue, charResult);

    //    public Character getCharacter() {return c;}
    InstanceMethod<void, jboxedchar> jSetChar("dev.tmich.jnipp.test.TestBoxed", "setCharacter");
    jSetChar(instance, kDefaultCharValue);
    charResult = jGetChar(instance);
    ASSERT_EQ(kDefaultCharValue, charResult);

    //    public Character minusCharacter(Character inval) { return (char) (c - inval); }
    InstanceMethod<jboxedchar, jboxedchar> jMinusChar("dev.tmich.jnipp.test.TestBoxed", "minusCharacter");
    charResult = jMinusChar(instance, 3);
    ASSERT_EQ('4', charResult);
    
    //    private Float f = 7.0F;
    const float kDefaultFloatValue = 7.0F;
    InstanceField<jboxedfloat> jFloatField("dev.tmich.jnipp.test.TestBoxed", "f");
    float floatResult = jFloatField.get(instance);
    ASSERT_FLOAT_EQ(kDefaultFloatValue, floatResult);
    const float kNewFloatValue = 7.777'777;
    jFloatField.set(instance, kNewFloatValue);

    //    public Float getFloat() {return f;}
    InstanceMethod<jboxedfloat> jGetFloat("dev.tmich.jnipp.test.TestBoxed", "getFloat");
    floatResult = jGetFloat(instance);
    ASSERT_FLOAT_EQ(kNewFloatValue, floatResult);

    //    public void setFloat(Float f) {this.f = f;}
    InstanceMethod<void, jboxedfloat> jSetFloat("dev.tmich.jnipp.test.TestBoxed", "setFloat");
    jSetFloat(instance, kDefaultFloatValue);
    floatResult = jGetFloat(instance);
    ASSERT_FLOAT_EQ(kDefaultFloatValue, floatResult);

    //    public Float timesFloat(Float inval) { return inval * f; }
    InstanceMethod<jboxedfloat, jboxedfloat> jTimesFloat("dev.tmich.jnipp.test.TestBoxed", "timesFloat");
    floatResult = jTimesFloat(instance, 3);
    ASSERT_FLOAT_EQ(kDefaultFloatValue*3, floatResult);
    
    //    private Double d = 7.777777777;
    const double kDefaultDoubleValue = 7.777'777'777;
    InstanceField<jboxeddouble> jDoubleField("dev.tmich.jnipp.test.TestBoxed", "d");
    double doubleResult = jDoubleField.get(instance);
    ASSERT_DOUBLE_EQ(kDefaultDoubleValue, doubleResult);
    const double kNewDoubleValue = 7'777'777;
    jDoubleField.set(instance, kNewDoubleValue);

    //    public Double getDouble() {return d;}
    InstanceMethod<jboxeddouble> jGetDouble("dev.tmich.jnipp.test.TestBoxed", "getDouble");
    doubleResult = jGetDouble(instance);
    ASSERT_DOUBLE_EQ(kNewDoubleValue, doubleResult);

    //    public void setDouble(Double d) {this.d = d;}
    InstanceMethod<void, jboxeddouble> jSetDouble("dev.tmich.jnipp.test.TestBoxed", "setDouble");
    jSetDouble(instance, kDefaultDoubleValue);
    doubleResult = jGetDouble(instance);
    ASSERT_DOUBLE_EQ(kDefaultDoubleValue, doubleResult);

    //    public Double timesDouble(Double inval) { return inval * d; }
    InstanceMethod<jboxeddouble, jboxeddouble> jTimesDouble("dev.tmich.jnipp.test.TestBoxed", "timesDouble");
    doubleResult = jTimesDouble(instance, 3);
    ASSERT_DOUBLE_EQ(kDefaultDoubleValue*3, doubleResult);
    
    //    private Boolean z = true;
    const bool kDefaultBoolValue = true;
    InstanceField<jboxedboolean> jBoolField("dev.tmich.jnipp.test.TestBoxed", "z");
    bool boolResult = jBoolField.get(instance);
    ASSERT_EQ(kDefaultBoolValue, boolResult);
    const bool kNewBoolValue = false;
    jBoolField.set(instance, kNewBoolValue);

    //    public Boolean getBoolean() {return z;}
    InstanceMethod<jboxedboolean> jGetBool("dev.tmich.jnipp.test.TestBoxed", "getBoolean");
    boolResult = jGetBool(instance);
    ASSERT_EQ(kNewBoolValue, boolResult);

    //    public void setBoolean(Boolean z) {this.z = z;}
    InstanceMethod<void, jboxedboolean> jSetBool("dev.tmich.jnipp.test.TestBoxed", "setBoolean");
    jSetBool(instance, kDefaultBoolValue);
    boolResult = jGetBool(instance);
    ASSERT_EQ(kDefaultBoolValue, boolResult);

    //    public Boolean xorBool(Boolean b) { return b ^ z; }
    InstanceMethod<jboxedboolean, jboxedboolean> jXOrBool("dev.tmich.jnipp.test.TestBoxed", "xorBool");
    boolResult = jXOrBool(instance, true);
    ASSERT_EQ(false, boolResult);
    boolResult = jXOrBool(instance, false);
    ASSERT_EQ(true, boolResult);
}
