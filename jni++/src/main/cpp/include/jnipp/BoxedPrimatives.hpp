//
// BoxedPrimatives.hpp
// libjni++
//
// Created by Thomas Micheline Apr 19, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//


#pragma once

#include "JniPlusPlus.hpp"
#include "jnipp/JniMapping.hpp"

namespace jni_pp {

typedef JniMapping<short, "java.lang.Short"> jboxedshort;
typedef JniMapping<int, "java.lang.Integer"> jboxedint;
typedef JniMapping<long, "java.lang.Long"> jboxedlong;
typedef JniMapping<char, "java.lang.Character"> jboxedchar;
typedef JniMapping<unsigned char, "java.lang.Byte"> jboxedbyte;
typedef JniMapping<float, "java.lang.Float"> jboxedfloat;
typedef JniMapping<double, "java.lang.Double"> jboxeddouble;
typedef JniMapping<bool, "java.lang.Boolean"> jboxedboolean;

template<>
inline short ToCppConverter<jboxedshort>::convertToCpp(jobject val) {
    static InstanceMethod<short> jShortValue("java.lang.Short", "shortValue");
    return jShortValue(val);
}
template<>
inline jobject ToJavaConverter<jboxedshort>::convertToJava(short value) {
    static StaticMethod<jobject, short> jShortValueOf("java.lang.Short", "valueOf", "(S)Ljava/lang/Short;");
    return jShortValueOf(value);
}

template<>
inline int ToCppConverter<jboxedint>::convertToCpp(jobject val) {
    static InstanceMethod<int> jIntegerValue("java.lang.Integer", "intValue");
    return jIntegerValue(val);
}
template<>
inline jobject ToJavaConverter<jboxedint>::convertToJava(int value) {
    static StaticMethod<jobject, int> jIntegerValueOf("java.lang.Integer", "valueOf", "(I)Ljava/lang/Integer;");
    return jIntegerValueOf(value);
}

template<>
inline long ToCppConverter<jboxedlong>::convertToCpp(jobject val) {
    static InstanceMethod<long> jLongValue("java.lang.Long", "longValue");
    return jLongValue(val);
}
template<>
inline jobject ToJavaConverter<jboxedlong>::convertToJava(long value) {
    static StaticMethod<jobject, long> jLongValueOf("java.lang.Long", "valueOf", "(J)Ljava/lang/Long;");
    return jLongValueOf(value);
}

template<>
inline char ToCppConverter<jboxedchar>::convertToCpp(jobject val) {
    static InstanceMethod<char> jCharacterValue("java.lang.Character", "charValue");
    return jCharacterValue(val);
}
template<>
inline jobject ToJavaConverter<jboxedchar>::convertToJava(char value) {
    static StaticMethod<jobject, char> jCharacterValueOf("java.lang.Character", "valueOf", "(C)Ljava/lang/Character;");
    return jCharacterValueOf(value);
}

template<>
inline unsigned char ToCppConverter<jboxedbyte>::convertToCpp(jobject val) {
    static InstanceMethod<unsigned char> jByteValue("java.lang.Byte", "byteValue");
    return jByteValue(val);
}
template<>
inline jobject ToJavaConverter<jboxedbyte>::convertToJava(unsigned char value) {
    static StaticMethod<jobject, unsigned char> jByteValueOf("java.lang.Byte", "valueOf", "(B)Ljava/lang/Byte;");
    return jByteValueOf(value);
}

template<>
inline float ToCppConverter<jboxedfloat>::convertToCpp(jobject val) {
    static InstanceMethod<float> jFloatValue("java.lang.Float", "floatValue");
    return jFloatValue(val);
}
template<>
inline jobject ToJavaConverter<jboxedfloat>::convertToJava(float value) {
    static StaticMethod<jobject, float> jFloatValueOf("java.lang.Float", "valueOf", "(F)Ljava/lang/Float;");
    return jFloatValueOf(value);
}

template<>
inline double ToCppConverter<jboxeddouble>::convertToCpp(jobject val) {
    static InstanceMethod<double> jDoubleValue("java.lang.Double", "doubleValue");
    return jDoubleValue(val);
}
template<>
inline jobject ToJavaConverter<jboxeddouble>::convertToJava(double value) {
    static StaticMethod<jobject, double> jDoubleValueOf("java.lang.Double", "valueOf", "(D)Ljava/lang/Double;");
    return jDoubleValueOf(value);
}

template<>
inline bool ToCppConverter<jboxedboolean>::convertToCpp(jobject val) {
    static InstanceMethod<bool> jBooleanValue("java.lang.Boolean", "booleanValue");
    return jBooleanValue(val);
}
template<>
inline jobject ToJavaConverter<jboxedboolean>::convertToJava(bool value) {
    static StaticMethod<jobject, bool> jBooleanValueOf("java.lang.Boolean", "valueOf", "(Z)Ljava/lang/Boolean;");
    return jBooleanValueOf(value);
}

}