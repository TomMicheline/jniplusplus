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
#include <JniPlusPlus.hpp>

#include "Converters.hpp"

#define USE_CXX_20_NONTYPE_TEMPLATE_PARAMETERS

namespace jni_pp {

#ifdef USE_CXX_20_NONTYPE_TEMPLATE_PARAMETERS

template<size_t N>
struct JvmClassName {
    constexpr JvmClassName(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }

    char value[N];

    operator const char *() {
        return value;
    }
};

#else

typedef const char *JvmClassName;

#endif

template<typename CppType, JvmClassName name>
struct JniMapping {
};

template<typename CppType, JvmClassName name>
struct Actualized<JniMapping<CppType, name>> {
    typedef CppType type;
};
template<typename CppType, JvmClassName name>
struct CppToJava<JniMapping<CppType, name>> {
    typedef jobject type;
};





//#################################################################################################
//#################################################################################################
//########
//########                  Implementation details
//########
//#################################################################################################
//#################################################################################################


}
