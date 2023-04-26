//
// JavaObjects.hpp
// libjni++
//
// Created by Thomas Micheline Apr 17, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#pragma once

#include <string>

#include "jnipp/Converters.hpp"
#include "jnipp/Utilities.hpp"

#define USE_CXX_20_NONTYPE_TEMPLATE_PARAMETERS

namespace jni_pp {

#ifdef USE_CXX_20_NONTYPE_TEMPLATE_PARAMETERS

template<size_t N>
struct LiteralName {
    constexpr LiteralName(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }
    char value[N];
};

template<LiteralName name>
std::string getName() {
    return std::string(name.value);
}

#else

typedef const char *LiteralName;

template<LiteralName name>
std::string getName() {
    return std::string(name);
}

#endif

template<LiteralName name, bool global = false>
struct JvmObject {
};

template<LiteralName name, bool global>
struct IsGlobalRef<JvmObject<name, global>> {
    static constexpr bool value = global;
};

template<LiteralName name, bool global>
struct JniSignature<JvmObject<name, global>> {
    std::string signature() {
        return JvmClassNameToJniSignature(typeName());
    }
    std::string typeName() {
        return getName<name>();
    }
};
template<LiteralName name, bool global>
struct JniTypeMapping<JvmObject<name, global>> {
    using actualCppType = jobject;
    using jniType = jobject;
};
template<LiteralName name, bool global>
struct ToCppConverter<JvmObject<name, global>> {
    static jobject convertToCpp(jobject val) {
        return val;
    }
};
template<LiteralName name, bool global>
struct ToJavaConverter<JvmObject<name, global>> {
    static jobject convertToJava(jobject value) {
        return value;
    }
};


template<typename CppType, LiteralName name>
struct JniMapping {
};

template<typename CppType, LiteralName name>
struct JniSignature<JniMapping<CppType, name>> {
    std::string signature() {
        return JvmClassNameToJniSignature(typeName());
    }
    std::string typeName() {
        return getName<name>();
    }
};

template<typename CppType, LiteralName name>
struct JniTypeMapping<JniMapping<CppType, name>> {
    using actualCppType = CppType;
    using jniType = jobject;
};





//#################################################################################################
//#################################################################################################
//########
//########                  Implementation details
//########
//#################################################################################################
//#################################################################################################


}
