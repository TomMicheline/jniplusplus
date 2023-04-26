//
// HighLevelInvokers.hpp
// jni++
//
// Created by Thomas Micheline Jun 22, 2013.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
// Copyright © 2023 VSee Lab, Inc. All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#pragma once

#include <jni.h>

#include "jnipp/Utilities.hpp"
#include "jnipp/Converters.hpp"
#include "jnipp/InvokersLowLevel.hpp"
#include "jnipp/References.hpp"


namespace jni_pp {

template <typename CppReturnType>
struct HighLevelInvoker {
    template<typename TargetType>
    static typename JniTypeMapping<CppReturnType>::actualCppType
    invoke(TargetType target, jmethodID methodID, vector <jvalue> &args, JniLocalReferenceScope &refs);
};

template <typename CppType>
struct HighLevelAccessor {
    using actualCppType = typename JniTypeMapping<CppType>::actualCppType;
    using jniType = typename JniTypeMapping<CppType>::jniType ;

    template <typename TargetType>
    static actualCppType get(TargetType target, jfieldID fieldID, JniLocalReferenceScope& refs);

    template <typename TargetType>
    static void set(TargetType target, jfieldID fieldID, actualCppType value);
};



//#################################################################################################
//#################################################################################################
//########
//########                  Implementation details
//########
//#################################################################################################
//#################################################################################################



template <typename CppReturnType>
template <typename TargetType>
typename JniTypeMapping<CppReturnType>::actualCppType HighLevelInvoker<CppReturnType>::invoke(TargetType target, jmethodID methodID, vector<jvalue>& args, JniLocalReferenceScope& refs) {
    using cppType = typename JniTypeMapping<CppReturnType>::actualCppType;
    using jniType = typename JniTypeMapping<CppReturnType>::jniType;
    jniType javaReturnValue = LowLevelInvoker<jniType>::invoke(target, methodID, args);
    checkForExceptions();
    cppType returnValue = ToCppConverter<CppReturnType>::convertToCpp(javaReturnValue);
    return JvmObjectPassThrough<cppType, IsGlobalRef<CppReturnType>::value>::pass(returnValue, refs);
};


template <>
template <typename TargetType>
inline void HighLevelInvoker<void>::invoke(TargetType target, jmethodID methodID, vector<jvalue>& args, JniLocalReferenceScope& refs) {
	LowLevelInvoker<void>::invoke(target, methodID, args);
	checkForExceptions();
}

template <typename CppValueType>
template <typename TargetType>
typename JniTypeMapping<CppValueType>::actualCppType HighLevelAccessor<CppValueType>::get(TargetType target, jfieldID fieldID, JniLocalReferenceScope& refs) {
    jniType javaReturnValue = LowLevelAccessor<jniType>::get(target, fieldID);
    checkForExceptions();
    actualCppType returnValue = ToCppConverter<CppValueType>::convertToCpp(javaReturnValue);
    return JvmObjectPassThrough<actualCppType, IsGlobalRef<CppValueType>::value>::pass(returnValue, refs);
}


template <typename CppValueType>
template <typename TargetType>
void HighLevelAccessor<CppValueType>::set(TargetType target, jfieldID fieldID, typename JniTypeMapping<CppValueType>::actualCppType value) {
    jniType javaValue = ToJavaConverter<CppValueType>::convertToJava(value);
    LowLevelAccessor<jniType>::set(target, fieldID, javaValue);
    checkForExceptions();
}


} // namespace jni_pp
