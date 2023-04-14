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
    static typename Actualized<CppReturnType>::type
    invoke(TargetType target, jmethodID methodID, vector <jvalue> &args, JniLocalReferenceScope &refs);
};

template <typename CppType>
struct HighLevelAccessor {
    template <typename TargetType>
    static typename Actualized<CppType>::type get(TargetType target, jfieldID fieldID, JniLocalReferenceScope& refs);

    template <typename TargetType>
    static void set(TargetType target, jfieldID fieldID, CppType value);
};



/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
//////////
//////////
//////////  Implementation details
//////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////



template <typename CppReturnType>
template <typename TargetType>
typename Actualized<CppReturnType>::type HighLevelInvoker<CppReturnType>::invoke(TargetType target, jmethodID methodID, vector<jvalue>& args, JniLocalReferenceScope& refs) {
    typedef typename Actualized<CppReturnType>::type ActualReturnType;
    typedef typename CppToJava<ActualReturnType>::type JavaReturnType;
    JavaReturnType javaReturnValue = LowLevelInvoker<JavaReturnType>::invoke(target, methodID, args);
    checkForExceptions();
    ActualReturnType returnValue = ToCppConverter<ActualReturnType>::convertToCpp(javaReturnValue);
    return Actualizer<CppReturnType>::passThroughOrConvert(returnValue, refs);
};


template <>
template <typename TargetType>
inline void HighLevelInvoker<void>::invoke(TargetType target, jmethodID methodID, vector<jvalue>& args, JniLocalReferenceScope& refs) {
	LowLevelInvoker<void>::invoke(target, methodID, args);
	checkForExceptions();
}

template <typename CppValueType>
template <typename TargetType>
typename Actualized<CppValueType>::type HighLevelAccessor<CppValueType>::get(TargetType target, jfieldID fieldID, JniLocalReferenceScope& refs) {
    typedef typename Actualized<CppValueType>::type ActualReturnType;
    typedef typename CppToJava<ActualReturnType>::type JavaReturnType;
    JavaReturnType javaReturnValue = LowLevelAccessor<JavaReturnType>::get(target, fieldID);
    checkForExceptions();
    ActualReturnType returnValue = ToCppConverter<ActualReturnType>::convertToCpp(javaReturnValue);
    return Actualizer<CppValueType>::passThroughOrConvert(returnValue, refs);
}


template <typename CppValueType>
template <typename TargetType>
void HighLevelAccessor<CppValueType>::set(TargetType target, jfieldID fieldID, CppValueType value) {
    typedef typename CppToJava<CppValueType>::type JavaValueType;
    JavaValueType javaValue = ToJavaConverter<JavaValueType>::convertToJava(value);
    LowLevelAccessor<JavaValueType>::set(target, fieldID, javaValue);
    checkForExceptions();
}


} // namespace jni_pp
