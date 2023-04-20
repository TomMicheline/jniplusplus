//
// SwigMethods.hpp
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

template<typename CppType, JvmClassName name>
struct SwigMapping {
};

template<typename CppType, JvmClassName name>
struct Actualized<SwigMapping<CppType, name>> {
    typedef CppType type;
};
template<typename CppType, JvmClassName name>
struct CppToJava<SwigMapping<CppType, name>> {
    typedef jobject type;
};

template<typename CppType, JvmClassName name>
struct ToCppConverter<SwigMapping<CppType, name>> {
    static CppType convertToCpp(jobject val) {
        static InstanceField<long> pointerField("", "ptr");
        auto pointerAsLong = pointerField.get(val);
        return **((CppType **) &pointerAsLong);
    }
};
template<typename CppType, JvmClassName name>
struct ToJavaConverter<SwigMapping<CppType, name>> {
    static jobject convertToJava(CppType val) {
        static Constructor<jobject, long, bool> ctor(name);
        return ctor(val, true);
    }
};


template <typename ReturnType, typename SwigClass>
class SwigConstructor : public Constructor<ReturnType, long, bool> {
public:
    SwigConstructor(const std::string& className) : Constructor<ReturnType, long, bool>(getSwigPackage() + "." + className, "(JZ)V") {}
    SwigConstructor() : Constructor<ReturnType, long, bool>(SwigWrapper<SwigClass>::getClassName(), "(JZ)V") {}

    typename Actualized<ReturnType>::type operator()(SwigClass* ptr, bool ownMemory) {
        return Method<ReturnType, long, bool>::operator()(reinterpret_cast<long>(ptr), ownMemory);
    }
    typename Actualized<ReturnType>::type operator()(SwigClass& ref) {
        return Method<ReturnType, long, bool>::operator()(reinterpret_cast<long>(&ref), false);
    }

    static typename Actualized<ReturnType>::type construct(SwigClass* ptr, bool ownMem);
    static typename Actualized<ReturnType>::type construct(SwigClass& ref);
};

template <typename ReturnType>
class SwigEnum : public StaticMethod<ReturnType, int> {
public:
    SwigEnum(const std::string& className) : StaticMethod<ReturnType, int>(getSwigPackage() + className, "swigToEnum") {}
};

template <typename CppElementType>
class SwigObjectArray {
public:
    SwigObjectArray() : className(SwigWrapper<CppElementType>::getClassName()) {}
    SwigObjectArray(const std::string& className) : className(getSwigPackage() + "." + className) {}

    jclass getClass() {
        call_once(runOnce, [&]{
            class_ = jni_pp::getClass(className);
        });
        return class_;
    }

    template <typename ReturnType>
    jobjectArray create(int size, SwigPtr<CppElementType> initialElement = getNullSwigPointer<CppElementType>());
    template <typename ReturnType>
    jobjectArray create(int size, SwigRef<CppElementType> initialElement);
    template <typename ReturnType, typename ElementScopeType>
    jobjectArray create(CppElementType cppArray[], int len);

    CppElementType* get(jobjectArray array, int index);

    template <typename ElementScopeType>
    void set(jobjectArray array, int index, SwigPtr<CppElementType> value);
    template <typename ElementScopeType>
    void set(jobjectArray array, int index, SwigRef<CppElementType> value);

private:
    std::string className;

    jclass class_;
    std::once_flag runOnce;
};

//#################################################################################################
//#################################################################################################
//########
//########                  Implementation details
//########
//#################################################################################################
//#################################################################################################

template<typename CppElementType>
template<typename ReturnType>
jobjectArray
SwigObjectArray<CppElementType>::create(int size, SwigPtr<CppElementType> initialElement) {
    JniLocalReferenceScope refs;
    jobject jInitial = nullptr;
    if (initialElement.value) {
        jInitial = SwigConstructor<ReturnType, CppElementType>::construct(initialElement.value,
                                                                          initialElement.ownsMem);
    }
    jarray array = env()->NewObjectArray(size, getClass(), jInitial);
    return static_cast<jobjectArray>(Actualizer<ReturnType>::passThroughOrConvert(array, refs));
}

template<typename CppElementType>
template<typename ReturnType>
jobjectArray
SwigObjectArray<CppElementType>::create(int size, SwigRef<CppElementType> initialElement) {
    JniLocalReferenceScope refs;
    jobject jInitial = SwigConstructor<ReturnType, CppElementType>::construct(initialElement.value);
    jarray array = env()->NewObjectArray(size, getClass(), jInitial);
    return static_cast<jobjectArray>(Actualizer<ReturnType>::passThroughOrConvert(array, refs));
}

template<typename CppElementType>
template<typename ReturnType, typename ElementScopeType>
jobjectArray SwigObjectArray<CppElementType>::create(CppElementType cppArray[], int len) {
    jobjectArray result = create<ReturnType>(len);
    for (auto i = 0; i < len; ++i) {
        set<ElementScopeType>(result, i, SwigRef<CppElementType>(cppArray[i]));
    }
    return result;
}

template<typename CppElementType>
CppElementType *SwigObjectArray<CppElementType>::get(jobjectArray array, int index) {
    JniLocalReferenceScope refs;
    jobject javaReturn = env()->GetObjectArrayElement(array, index);
    return getSwigPointer<CppElementType>(javaReturn);
}

template<typename CppElementType>
template<typename ElementScopeType>
void
SwigObjectArray<CppElementType>::set(jobjectArray array, int index, SwigPtr<CppElementType> value) {
    JniLocalReferenceScope refs;
    jobject javaValue = SwigConstructor<ElementScopeType, CppElementType>::construct(value.value,
                                                                                     value.ownsMem);
    env()->SetObjectArrayElement(array, index, javaValue);
}

template<typename CppElementType>
template<typename ElementScopeType>
void
SwigObjectArray<CppElementType>::set(jobjectArray array, int index, SwigRef<CppElementType> value) {
    JniLocalReferenceScope refs;
    jobject javaValue = SwigConstructor<ElementScopeType, CppElementType>::construct(value.value);
    env()->SetObjectArrayElement(array, index, javaValue);
}

template<typename ReturnType, typename SwigClass>
typename Actualized<ReturnType>::type
SwigConstructor<ReturnType, SwigClass>::construct(SwigClass *ptr, bool ownMem) {
    static SwigConstructor<ReturnType, SwigClass> ctor;
    return ctor(ptr, ownMem);
}

template<typename ReturnType, typename SwigClass>
typename Actualized<ReturnType>::type
SwigConstructor<ReturnType, SwigClass>::construct(SwigClass &ref) {
    static SwigConstructor<ReturnType, SwigClass> ctor;
    return ctor(ref);
}

}
