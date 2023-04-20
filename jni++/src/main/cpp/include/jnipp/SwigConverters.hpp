//
// SwigConverters.hpp
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

#include "Converters.hpp"

namespace jni_pp {

template <typename CppType>
struct SwigWrapper : WrapperMapper {
public:
    static void mapTo(const std::string& className);
    static const std::string& getClassName();
    static bool isMapped();
};

template <typename CppType>
struct SwigPtr {
    SwigPtr(CppType* ptr, bool passOwnership) : value(ptr), ownsMem(passOwnership) {}
    CppType* value;
    bool ownsMem;
};

template <typename CppType>
struct SwigRef {
    SwigRef(CppType& ref) : value(ref) {}
    CppType& value;
};

template <typename CppElementType>
struct SwigArray {
    SwigArray(CppElementType* arry, int len, bool globalElements = true) : value(arry), length(len), globalElements(globalElements) {}

    int length;
    bool globalElements;
    CppElementType* value;
};

template <typename CppType>
SwigPtr<CppType> getNullSwigPointer() {
    return SwigPtr<CppType>(nullptr, false);
}

template <typename CppType> struct Actualized<SwigWrapper<CppType>> { typedef CppType* type; };

template <typename CppReturnType>
struct ToCppConverter<SwigWrapper<CppReturnType>> {
    template <typename CppType>
    static CppReturnType* convertToCpp(jobject val) {
        return getSwigPointer<CppReturnType>(val);
    }
};

//#################################################################################################
//#################################################################################################
//########
//########                  Implementation details
//########
//#################################################################################################
//#################################################################################################


template<typename CppType>
void SwigWrapper<CppType>::mapTo(const std::string &className) {
    std::lock_guard <std::mutex> lg(mutex);
    auto index = std::type_index(typeid(CppType));
    wrapperMap.emplace(index, getSwigPackage() + "." + className);
}

template<typename CppType>
const std::string &SwigWrapper<CppType>::getClassName() {
    std::lock_guard <std::mutex> lg(mutex);
    auto &ti = typeid(CppType);
    auto index = std::type_index(ti);
    auto nameIt = wrapperMap.find(index);
    if (nameIt == wrapperMap.end()) throw std::runtime_error(
                std::string("Attempt to construct SWIG class with no mapping registered: ") +
                ti.name());
    return wrapperMap[index];
}

template<typename CppType>
bool SwigWrapper<CppType>::isMapped() {
    std::lock_guard <std::mutex> lg(mutex);
    auto &ti = typeid(CppType);
    auto index = std::type_index(ti);
    auto nameIt = wrapperMap.find(index);
    return nameIt != wrapperMap.end();
}


template<typename CppType>
jvalue convertToJValue(SwigPtr<CppType> arg);

template<typename CppType>
jvalue convertToJValue(SwigRef<CppType> arg);

template<typename CppElementType>
jvalue convertToJValue(SwigArray<CppElementType> arg);

template<typename CppType>
jvalue convertToJValue(SwigRef<CppType> arg) {
    static SwigConstructor <jlocal, CppType> ctor;
    return convertToJValue(ctor(arg.value));
}

template<typename CppType>
jvalue convertToJValue(SwigPtr<CppType> ptr) {
    static SwigConstructor <jlocal, CppType> ctor;
    return convertToJValue(ctor(ptr.value, ptr.ownsMem));
}

template<typename CppElementType>
jvalue convertToJValue(SwigArray<CppElementType> arry) {
    static SwigObjectArray <CppElementType> creator;
    jobjectArray result;
    if (arry.globalElements)
        result = creator.template create<jlocal, jglobal>(arry.value, arry.length);
    else
        result = creator.template create<jlocal, jlocal>(arry.value, arry.length);
    return convertToJValue(result);
}

template<typename CppType>
CppType *getSwigPointer(jobject obj) {
    static InstanceField<long> pointerField(SwigWrapper<CppType>::getClassName());
    auto pointerAsLong = pointerField.get(obj);
    return *((CppType **) &pointerAsLong);
//    return reinterpret_cast<CppType*>(pointerAsLong);
}

}
