//
// SwigSupport.hpp
// jni++
//
// Created by Thomas Micheline Apr 16, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#pragma once

#include <jni.h>
#include <string>

#include "JniPlusPlus.hpp"
#include "jnipp/Converters.hpp"
#include "jnipp/JniMapping.hpp"


namespace jni_pp {

template<typename CppType, LiteralName name>
struct SwigMapping {
};

template<typename CppType, LiteralName name>
struct JniSignature<SwigMapping<CppType, name>> {
    std::string signature() {
        return JvmClassNameToJniSignature(typeName());
    }
    std::string typeName() {
        return getName<name>();
    }
};

template<typename CppType, LiteralName name>
struct JniTypeMapping<SwigMapping<CppType, name>> {
    using actualCppType = CppType*;
    using jniType = jobject;
};

template<typename CppType, LiteralName name>
struct ToCppConverter<SwigMapping<CppType, name>> {
    template<>
    static CppType* convertToCpp<jobject>(jobject val) {
        static InstanceField<long> pointerField("", "ptr");
        auto pointerAsLong = pointerField.get(val);
        return **((CppType **) &pointerAsLong);
    }
};
template<typename CppType, LiteralName name>
struct ToJavaConverter<SwigMapping<CppType, name>> {
    static jobject convertToJava(CppType* val) {
        JniLocalReferenceScope ref;
        static Constructor<jobject, long, bool> ctor(getName<name>());
        jobject result = ctor(reinterpret_cast<long>(val), true);
        return JvmObjectPassThrough<jobject, false>::pass(result, ref);
    }
};

/*
template <typename ReturnType, typename SwigClass>
class SwigConstructor : public Constructor<ReturnType, long, bool> {
public:
    SwigConstructor(const std::string& className) : Constructor<ReturnType, long, bool>(getSwigPackage() + "." + className, "(JZ)V") {}
    SwigConstructor() : Constructor<ReturnType, long, bool>(SwigWrapper<SwigClass>::getClassName(), "(JZ)V") {}

    typename JniTypeMapping<ReturnType>::actualCppType operator()(SwigClass* ptr, bool ownMemory) {
        return Method<ReturnType, long, bool>::operator()(reinterpret_cast<long>(ptr), ownMemory);
    }
    typename JniTypeMapping<ReturnType>::actualCppType operator()(SwigClass& ref) {
        return Method<ReturnType, long, bool>::operator()(reinterpret_cast<long>(&ref), false);
    }

    static typename JniTypeMapping<ReturnType>::actualCppType construct(SwigClass* ptr, bool ownMem);
    static typename JniTypeMapping<ReturnType>::actualCppType construct(SwigClass& ref);
};

template <typename ReturnType>
class SwigEnum : public StaticMethod<ReturnType, int> {
public:
    SwigEnum(const std::string& className) : StaticMethod<ReturnType, int>(getSwigPackage() + className, "swigToEnum") {}
};
*/

//#################################################################################################
//#################################################################################################
//########
//########                  Implementation details
//########
//#################################################################################################
//#################################################################################################

/*
template<typename ReturnType, typename SwigClass>
typename JniTypeMapping<ReturnType>::actualCppType
SwigConstructor<ReturnType, SwigClass>::construct(SwigClass *ptr, bool ownMem) {
    static SwigConstructor<ReturnType, SwigClass> ctor;
    return ctor(ptr, ownMem);
}

template<typename ReturnType, typename SwigClass>
typename JniTypeMapping<ReturnType>::actualCppType
SwigConstructor<ReturnType, SwigClass>::construct(SwigClass &ref) {
    static SwigConstructor<ReturnType, SwigClass> ctor;
    return ctor(ref);
}
*/

}
