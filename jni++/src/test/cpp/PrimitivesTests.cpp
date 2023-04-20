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


TEST_F(JvmTestFixture, PrimitiviesStaticTests)
{
    StaticMethod<int> jGetInt("dev.tmich.jnipp.test.TestStaticPrimitives", "getInt");
    int getIntResult = jGetInt();
    ASSERT_EQ(7, getIntResult) << "TestStaticPrimitives.getInt() should return 7 but returned " << getIntResult;

    StaticMethod<int, int> jTimesTwoInt("dev.tmich.jnipp.test.TestStaticPrimitives", "timesTwoInt");
    int getTwoTimesIntResult = jTimesTwoInt(12);
    ASSERT_EQ(12*2, getTwoTimesIntResult) << "TestStaticPrimitives.timesTwoInt() should return " << (2*12) << " but returned " << getTwoTimesIntResult;

    StaticMethod<short> jGetShort("dev.tmich.jnipp.test.TestStaticPrimitives", "getShort");
    short getShortResult = jGetShort();
    ASSERT_EQ(7, getShortResult) << "TestStaticPrimitives.getShort() should return 7 but returned " << getShortResult;

    StaticMethod<short, short> jTimesTwoShort("dev.tmich.jnipp.test.TestStaticPrimitives", "timesTwoShort");
    short getTwoTimesShortResult = jTimesTwoShort(37);
    ASSERT_EQ(37*2, getTwoTimesShortResult) << "TestStaticPrimitives.timesTwoShort() should return " << (2*37) << " but returned " << getTwoTimesShortResult;

    const long kLongValue = 7'000'000'000'000;
    StaticMethod<long> jGetLong("dev.tmich.jnipp.test.TestStaticPrimitives", "getLong");
    long getLongResult = jGetLong();
    ASSERT_EQ(kLongValue, getLongResult) << "TestStaticPrimitives.getLong() should return " << kLongValue << " but returned " << getLongResult;

    StaticMethod<long, long> jTimesTwoLong("dev.tmich.jnipp.test.TestStaticPrimitives", "timesTwoLong");
    long getTwoTimesLongResult = jTimesTwoLong(kLongValue);
    ASSERT_EQ(kLongValue*2, getTwoTimesLongResult) << "TestStaticPrimitives.timesTwoLong() should return " << (kLongValue*12) << " but returned " << getTwoTimesLongResult;

    StaticMethod<float> jGetFloat("dev.tmich.jnipp.test.TestStaticPrimitives", "getFloat");
    float getFloatResult = jGetFloat();
    ASSERT_FLOAT_EQ(42.0F, getFloatResult) << "TestStaticPrimitives.getFloat() should return 42.0 but returned " << getFloatResult;

    const float kFloatValue = 127.0F;
    StaticMethod<float, float> jTimesFiveFloat("dev.tmich.jnipp.test.TestStaticPrimitives", "timesFiveFloat");
    float getFiveTimesFloatResult = jTimesFiveFloat(kFloatValue);
    ASSERT_EQ(kFloatValue*5, getFiveTimesFloatResult) << "TestStaticPrimitives.timesFiveFloat() should return " << (kFloatValue*5) << " but returned " << getFiveTimesFloatResult;

    StaticMethod<double> jGetDouble("dev.tmich.jnipp.test.TestStaticPrimitives", "getDouble");
    double getDoubleResult = jGetDouble();
    ASSERT_EQ(42.0, getDoubleResult) << "TestStaticPrimitives.getDouble() should return 42.0 but returned " << getDoubleResult;

    const double kDoubleValue = 578967.234567;
    StaticMethod<double, double> jTimesFiveDouble("dev.tmich.jnipp.test.TestStaticPrimitives", "timesFiveDouble");
    double getFiveTimesDoubleResult = jTimesFiveDouble(kDoubleValue);
    ASSERT_EQ(kDoubleValue*5, getFiveTimesDoubleResult) << "TestStaticPrimitives.timesFiveDouble() should return " << (kDoubleValue*5) << " but returned " << getFiveTimesDoubleResult;

    const unsigned char kByteValue = 0xA1;
    StaticMethod<unsigned char> jGetByte("dev.tmich.jnipp.test.TestStaticPrimitives", "getByte");
    unsigned char getByteResult = jGetByte();
    ASSERT_EQ(kByteValue, getByteResult) << "TestStaticPrimitives.getByte() should return " << kByteValue << " but returned " << getByteResult;

    StaticMethod<unsigned char, unsigned char> jTimesTwoByte("dev.tmich.jnipp.test.TestStaticPrimitives", "timesTwoByte");
    unsigned char getTwoTimesByteResult = jTimesTwoByte(12);
    ASSERT_EQ(12*2, getTwoTimesByteResult) << "TestStaticPrimitives.timesTwoByte() should return " << (2*12) << " but returned " << getTwoTimesByteResult;

    const char kCharValue = '!';
    StaticMethod<char> jGetChar("dev.tmich.jnipp.test.TestStaticPrimitives", "getChar");
    char getCharResult = jGetChar();
    ASSERT_EQ(kCharValue, getCharResult) << "TestStaticPrimitives.getChar() should return " << kCharValue << " but returned " << getCharResult;

    StaticMethod<char, char> jMinusOneChar("dev.tmich.jnipp.test.TestStaticPrimitives", "minusOneChar");
    char getMinusOneCharResult = jMinusOneChar('B');
    ASSERT_EQ('A', getMinusOneCharResult) << "TestStaticPrimitives.minusoneChar() should return 'A' but returned '" << char(getMinusOneCharResult) << "'";

    StaticMethod<bool> jGetBoolean("dev.tmich.jnipp.test.TestStaticPrimitives", "getBoolean");
    bool getBooleanResult = jGetBoolean();
    ASSERT_EQ(true, getBooleanResult) << "TestStaticPrimitives.getBoolean() should return true but returned " << getBooleanResult;

    StaticMethod<bool, bool> jNegateBoolean("dev.tmich.jnipp.test.TestStaticPrimitives", "negateBoolean");
    bool getNegateBooleanResult = jNegateBoolean(true);
    ASSERT_FALSE(getNegateBooleanResult) << "TestStaticPrimitives.negateBoolean() should return false but returned " << getNegateBooleanResult;

    const std::string kDefaultString("Welcome to the JVM");
    StaticMethod<std::string> jGetString("dev.tmich.jnipp.test.TestStaticPrimitives", "getString");
    std::string getStringResult = jGetString();
    ASSERT_EQ(kDefaultString, getStringResult) << "TestStaticPrimitives.getString() should return '" << kDefaultString << "' but returned '" << getStringResult << "'";

    const char* kOtherString = "ding dong ";
    StaticMethod<const char *, const char *> jTimesTwoString("dev.tmich.jnipp.test.TestStaticPrimitives", "timesTwoString");
    const char* getTwoTimesStringResult = jTimesTwoString(kOtherString);
    ASSERT_STREQ("ding dong ding dong ", getTwoTimesStringResult) << "TestStaticPrimitives.timesTwoString() should return 'ding dong ding dong ' but returned '" << getTwoTimesStringResult << "'";
}

TEST_F(JvmTestFixture, PrimitiviesInstanceTests) {
    Constructor<jobject> jCtor("dev.tmich.jnipp.test.TestInstancePrimitives");
    jobject instance = jCtor();

    //    private int i = 7_000_000;
    const int kDefaultIntValue = 7'000'000;
    InstanceField<int> jIntField("dev.tmich.jnipp.test.TestInstancePrimitives", "i");
    int intResult = jIntField.get(instance);
    ASSERT_EQ(kDefaultIntValue, intResult);
    const int kNewIntValue = 7'777'777;
    jIntField.set(instance, kNewIntValue);

    //    public int getInt() {return i;}
    InstanceMethod<int> jGetInt("dev.tmich.jnipp.test.TestInstancePrimitives", "getInt");
    intResult = jGetInt(instance);
    ASSERT_EQ(kNewIntValue, intResult);

    //    public void setInt(int i) {this.i = i;}
    InstanceMethod<void, int> jSetInt("dev.tmich.jnipp.test.TestInstancePrimitives", "setInt");
    jSetInt(instance, kDefaultIntValue);
    intResult = jGetInt(instance);
    ASSERT_EQ(kDefaultIntValue, intResult);

    //    public int timesInt(int inval) { return i * inval; }
    InstanceMethod<int, int> jTimesInt("dev.tmich.jnipp.test.TestInstancePrimitives", "timesInt");
    intResult = jTimesInt(instance, 3);
    ASSERT_EQ(3*kDefaultIntValue, intResult);
    
    //    private long l = 7_000_000_000_000L;
    const long kDefaultLongValue = 7'000'000'000'000;
    InstanceField<long> jLongField("dev.tmich.jnipp.test.TestInstancePrimitives", "l");
    long longResult = jLongField.get(instance);
    ASSERT_EQ(kDefaultLongValue, longResult);
    const long kNewLongValue = 7'777'777'777'777;
    jLongField.set(instance, kNewLongValue);

    //    public long getLong() {return l;}
    InstanceMethod<long> jGetLong("dev.tmich.jnipp.test.TestInstancePrimitives", "getLong");
    longResult = jGetLong(instance);
    ASSERT_EQ(kNewLongValue, longResult);

    //    public void setLong(long l) {this.l = l;}
    InstanceMethod<void, long> jSetLong("dev.tmich.jnipp.test.TestInstancePrimitives", "setLong");
    jSetLong(instance, kDefaultLongValue);
    longResult = jGetLong(instance);
    ASSERT_EQ(kDefaultLongValue, longResult);

    //    public long timesLong(long inval) { return l * inval; }
    InstanceMethod<long, long> jTimesLong("dev.tmich.jnipp.test.TestInstancePrimitives", "timesLong");
    longResult = jTimesLong(instance, 3);
    ASSERT_EQ(3*kDefaultLongValue, longResult);

    //    private short s = 7_000;
    const short kDefaultShortValue = 7'000;
    InstanceField<short> jShortField("dev.tmich.jnipp.test.TestInstancePrimitives", "s");
    short shortResult = jShortField.get(instance);
    ASSERT_EQ(kDefaultShortValue, shortResult);
    const short kNewShortValue = 7'777;
    jShortField.set(instance, kNewShortValue);

    //    public short getShort() {return s;}
    InstanceMethod<short> jGetShort("dev.tmich.jnipp.test.TestInstancePrimitives", "getShort");
    shortResult = jGetShort(instance);
    ASSERT_EQ(kNewShortValue, shortResult);

    //    public void setShort(short s) {this.s = s;}
    InstanceMethod<void, short> jSetShort("dev.tmich.jnipp.test.TestInstancePrimitives", "setShort");
    jSetShort(instance, kDefaultShortValue);
    shortResult = jGetShort(instance);
    ASSERT_EQ(kDefaultShortValue, shortResult);

    //    public short timesShort(short inval) { return (short) (s * inval); }
    InstanceMethod<short, short> jTimesShort("dev.tmich.jnipp.test.TestInstancePrimitives", "timesShort");
    shortResult = jTimesShort(instance, 3);
    ASSERT_EQ(3*kDefaultShortValue, shortResult);

    //    private byte b = 70;
    const unsigned char kDefaultByteValue = 70;
    InstanceField<unsigned char> jByteField("dev.tmich.jnipp.test.TestInstancePrimitives", "b");
    unsigned char byteResult = jByteField.get(instance);
    ASSERT_EQ(kDefaultByteValue, byteResult);
    const unsigned char kNewByteValue = 77;
    jByteField.set(instance, kNewByteValue);

    //    public byte getByte() {return b;}
    InstanceMethod<unsigned char> jGetByte("dev.tmich.jnipp.test.TestInstancePrimitives", "getByte");
    byteResult = jGetByte(instance);
    ASSERT_EQ(kNewByteValue, byteResult);

    //    public void setByte(byte b) {this.b = b;}
    InstanceMethod<void, unsigned char> jSetByte("dev.tmich.jnipp.test.TestInstancePrimitives", "setByte");
    jSetByte(instance, kDefaultByteValue);
    byteResult = jGetByte(instance);
    ASSERT_EQ(kDefaultByteValue, byteResult);

    //    public byte timesByte(byte inval) { return (byte) (b * inval); }
    InstanceMethod<unsigned char, unsigned char> jTimesByte("dev.tmich.jnipp.test.TestInstancePrimitives", "timesByte");
    byteResult = jTimesByte(instance, 3);
    ASSERT_EQ(3*kDefaultByteValue, byteResult);

    //    private char c = '7';
    const char kDefaultCharValue = '7';
    InstanceField<char> jCharField("dev.tmich.jnipp.test.TestInstancePrimitives", "c");
    char charResult = jCharField.get(instance);
    ASSERT_EQ(kDefaultCharValue, charResult);
    const char kNewCharValue = 'F';
    jCharField.set(instance, kNewCharValue);

    //    public char getChar() {return c;}
    InstanceMethod<char> jGetChar("dev.tmich.jnipp.test.TestInstancePrimitives", "getChar");
    charResult = jGetChar(instance);
    ASSERT_EQ(kNewCharValue, charResult);

    //    public void setChar(char c) {this.c = c;}
    InstanceMethod<void, char> jSetChar("dev.tmich.jnipp.test.TestInstancePrimitives", "setChar");
    jSetChar(instance, kDefaultCharValue);
    charResult = jGetChar(instance);
    ASSERT_EQ(kDefaultCharValue, charResult);

    //    public char minusChar(char inval) { return (char) (c - inval); }
    InstanceMethod<char, char> jMinusChar("dev.tmich.jnipp.test.TestInstancePrimitives", "minusChar");
    charResult = jMinusChar(instance, 3);
    ASSERT_EQ('4', charResult);

    //    private float f = 7.0F;
    const float kDefaultFloatValue = 7.0F;
    InstanceField<float> jFloatField("dev.tmich.jnipp.test.TestInstancePrimitives", "f");
    float floatResult = jFloatField.get(instance);
    ASSERT_FLOAT_EQ(kDefaultFloatValue, floatResult);
    const float kNewFloatValue = 7.777;
    jFloatField.set(instance, kNewFloatValue);

    //    public float getFloat() {return f;}
    InstanceMethod<float> jGetFloat("dev.tmich.jnipp.test.TestInstancePrimitives", "getFloat");
    floatResult = jGetFloat(instance);
    ASSERT_FLOAT_EQ(kNewFloatValue, floatResult);

    //    public void setFloat(float f) {this.f = f;}
    InstanceMethod<void, float> jSetFloat("dev.tmich.jnipp.test.TestInstancePrimitives", "setFloat");
    jSetFloat(instance, kDefaultFloatValue);
    floatResult = jGetFloat(instance);
    ASSERT_FLOAT_EQ(kDefaultFloatValue, floatResult);

    //    public float timesFloat(float inval) { return inval * f; }
    InstanceMethod<float, float> jTimesFloat("dev.tmich.jnipp.test.TestInstancePrimitives", "timesFloat");
    floatResult = jTimesFloat(instance, 3);
    ASSERT_FLOAT_EQ(3.0F*kDefaultFloatValue, floatResult);

    //    private double d = 7.777777777;
    const double kDefaultDoubleValue = 7.777777777;
    InstanceField<double> jDoubleField("dev.tmich.jnipp.test.TestInstancePrimitives", "d");
    double doubleResult = jDoubleField.get(instance);
    ASSERT_DOUBLE_EQ(kDefaultDoubleValue, doubleResult);
    const double kNewDoubleValue = 1234.123456789;
    jDoubleField.set(instance, kNewDoubleValue);

    //    public double getDouble() {return d;}
    InstanceMethod<double> jGetDouble("dev.tmich.jnipp.test.TestInstancePrimitives", "getDouble");
    doubleResult = jGetDouble(instance);
    ASSERT_DOUBLE_EQ(kNewDoubleValue, doubleResult);

    //    public void setDouble(double d) {this.d = d;}
    InstanceMethod<void, double> jSetDouble("dev.tmich.jnipp.test.TestInstancePrimitives", "setDouble");
    jSetDouble(instance, kDefaultDoubleValue);
    doubleResult = jGetDouble(instance);
    ASSERT_DOUBLE_EQ(kDefaultDoubleValue, doubleResult);

    //    public double timesDouble(double inval) { return inval * d; }
    InstanceMethod<double, double> jTimesDouble("dev.tmich.jnipp.test.TestInstancePrimitives", "timesDouble");
    doubleResult = jTimesDouble(instance, 3);
    ASSERT_DOUBLE_EQ(3.0*kDefaultDoubleValue, doubleResult);

    //    private boolean bool = false;
    const bool kDefaultBoolValue = false;
    InstanceField<bool> jBoolField("dev.tmich.jnipp.test.TestInstancePrimitives", "bool");
    bool boolResult = jBoolField.get(instance);
    ASSERT_EQ(kDefaultBoolValue, boolResult);
    const bool kNewBoolValue = true;
    jBoolField.set(instance, kNewBoolValue);

    //    public boolean isBool() {return bool;}
    InstanceMethod<bool> jGetBool("dev.tmich.jnipp.test.TestInstancePrimitives", "isBool");
    boolResult = jGetBool(instance);
    ASSERT_EQ(kNewBoolValue, boolResult);

    //    public void setBool(boolean bool) {this.bool = bool;}
    InstanceMethod<void, bool> jSetBool("dev.tmich.jnipp.test.TestInstancePrimitives", "setBool");
    jSetBool(instance, kDefaultBoolValue);
    boolResult = jGetBool(instance);
    ASSERT_EQ(kDefaultBoolValue, boolResult);

    //    public boolean negateBoolean() { return !bool; }
    InstanceMethod<bool> jNegateBoolean("dev.tmich.jnipp.test.TestInstancePrimitives", "negateBoolean");
    boolResult = jNegateBoolean(instance);
    ASSERT_EQ(!kDefaultBoolValue, boolResult);

    //    private String str = "Welcome to the JVM";
    const std::string kDefaultStringValue = "Welcome to the JVM";
    InstanceField<std::string> jStringField("dev.tmich.jnipp.test.TestInstancePrimitives", "str");
    std::string stringResult = jStringField.get(instance);
    ASSERT_EQ(kDefaultStringValue, stringResult);
    std::string kNewStringValue =  "... new day";
    jStringField.set(instance, kNewStringValue);

    //    public String getStr() {return str;}
    InstanceMethod<std::string> jGetString("dev.tmich.jnipp.test.TestInstancePrimitives", "getStr");
    stringResult = jGetString(instance);
    ASSERT_EQ(kNewStringValue, stringResult);

    //    public void setStr(String str) {this.str = str;}
    InstanceMethod<void, std::string> jSetString("dev.tmich.jnipp.test.TestInstancePrimitives", "setStr");
    jSetString(instance, kDefaultStringValue);
    stringResult = jGetString(instance);
    ASSERT_EQ(kDefaultStringValue, stringResult);

    //    public String addString(String inval) { return str + inval; }
    InstanceMethod<std::string, std::string> jAddString("dev.tmich.jnipp.test.TestInstancePrimitives", "addString");
    stringResult = jAddString(instance, "... relax");
    ASSERT_EQ("Welcome to the JVM... relax", stringResult);
}
