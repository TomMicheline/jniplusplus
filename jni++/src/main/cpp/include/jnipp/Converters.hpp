//
// Converters.hpp
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
#include <typeindex>
#include <map>
#include <string>
#include <string.h>

#include "jnipp/Utilities.hpp"
#include "jnipp/References.hpp"


//
// Same as jobject but used to trigger conversion from local to global reference
//
class jglobal {};
typedef jobject jlocal;

namespace jni_pp {

struct WrapperMapper {
protected:
	static std::mutex mutex;
	static std::map<std::type_index, const std::string > wrapperMap;

};

template <typename DeclaredReturnType>
struct Actualized {
    typedef DeclaredReturnType type;
};
template<> struct Actualized<jglobal> { typedef jobject type; };

template<typename CppType>
struct CppToJava {
    typedef jobject type;
};

template<typename JavaType>
struct JavaToArray {
    typedef jarray type;
};

// implemented in JniUtilities.cpp
template <typename CppType>
CppType* getSwigPointer(jobject obj);

template <typename CppType>
struct ToCppConverter {
    template<typename JavaType>
    static typename Actualized<CppType>::type convertToCpp(JavaType val);
};
template <typename CppType>
struct ToJavaConverter {
    template <typename JavaType>
    static JavaType convertToJava(typename Actualized<CppType>::type value);
};


//#################################################################################################
//#################################################################################################
//########
//########                  Implementation details
//########
//#################################################################################################
//#################################################################################################


jvalue convertToJValue(const std::string& arg);
jvalue convertToJValue(const char * arg);
jvalue convertToJValue(bool arg);
jvalue convertToJValue(unsigned char arg);
jvalue convertToJValue(char arg);
jvalue convertToJValue(short arg);
jvalue convertToJValue(int arg);
jvalue convertToJValue(long arg);
jvalue convertToJValue(long long arg);
jvalue convertToJValue(float arg);
jvalue convertToJValue(double arg);
jvalue convertToJValue(int64_t arg);
jvalue convertToJValue(jobject arg);

jstring toJString(const char *s);
jstring toJString(const std::string& s);

inline std::string jStringToString(jstring s) {
    if (s == nullptr) return "";
	const char *c = env()->GetStringUTFChars(s, nullptr);
	std::string result(c);
	env()->ReleaseStringUTFChars(s, c);
	return result;
}


inline char * jStringToCharPtr(jstring s) {
    if (s == nullptr) return nullptr;
	const char *c = env()->GetStringUTFChars(s, nullptr);
	char *r = strdup(c);
	env()->ReleaseStringUTFChars(s, c);
	return r;
}

template <typename... Args>
struct GatherArguments;

template <typename ArgType, typename... Args>
struct GatherArguments<ArgType, Args...> {
    static void gather(vector<jvalue> &javaArgs, typename Actualized<ArgType>::type arg1, typename Actualized<Args>::type ...remainingArgs) {
        typedef typename CppToJava<ArgType>::type JavaType;
        jvalue jarg1 = convertToJValue(ToJavaConverter<ArgType>::template convertToJava<JavaType>(arg1));
        javaArgs.push_back(jarg1);
        GatherArguments<Args...>::gather(javaArgs, remainingArgs...);
    }
};

template <>
struct GatherArguments<> {
    static void gather(vector<jvalue> &javaArgs) {
    }
};

template<> struct CppToJava<std::string>  { typedef jobject type; };
template<> struct CppToJava<char *> { typedef jobject type; };
template<> struct CppToJava<bool> { typedef jboolean type; };
template<> struct CppToJava<unsigned char> { typedef jbyte type; };
template<> struct CppToJava<char> { typedef jchar type; };
template<> struct CppToJava<short> { typedef jshort type; };
template<> struct CppToJava<int> { typedef jint type; };
template<> struct CppToJava<long> { typedef jlong type; };
template<> struct CppToJava<long long> { typedef jlong type; };
template<> struct CppToJava<float> { typedef jfloat type; };
template<> struct CppToJava<double> { typedef jdouble type; };

template<> struct JavaToArray<jboolean> { typedef jbooleanArray type; };
template<> struct JavaToArray<jbyte> { typedef jbyteArray type; };
template<> struct JavaToArray<jchar> { typedef jcharArray type; };
template<> struct JavaToArray<jshort> { typedef jshortArray type; };
template<> struct JavaToArray<jint> { typedef jintArray type; };
template<> struct JavaToArray<jlong> { typedef jlongArray type; };
template<> struct JavaToArray<jfloat> { typedef jfloatArray type; };
template<> struct JavaToArray<jdouble> { typedef jdoubleArray type; };

template <typename CppType>
template <typename JavaType>
typename Actualized<CppType>::type ToCppConverter<CppType>::convertToCpp(JavaType val) {
    return CppType(val);
}

template <>
template <>
inline std::string ToCppConverter<std::string>::convertToCpp(jobject val) {
	std::string result = jStringToString(static_cast<jstring>(val));
	env()->DeleteLocalRef(val);
	return result;
}

template <>
template <>
inline char * ToCppConverter<char *>::convertToCpp(jobject val) {
    char * result = jStringToCharPtr(static_cast<jstring>(val));
    env()->DeleteLocalRef(val);
    return result;
}

template <>
template <>
inline const char * ToCppConverter<const char *>::convertToCpp(jobject val) {
    const char * result = jStringToCharPtr(static_cast<jstring>(val));
    env()->DeleteLocalRef(val);
    return result;
}

template <typename CppType>
template <typename JavaType>
JavaType ToJavaConverter<CppType>::convertToJava(typename Actualized<CppType>::type val) {
    return JavaType(val);
}

template <>
template <>
inline jchar ToJavaConverter<char>::convertToJava(char val) {
    return jchar(val);
}

template <>
template <>
inline jobject ToJavaConverter<std::string>::convertToJava(std::string val) {
    return toJString(val);
}

template <>
template <>
inline jobject ToJavaConverter<const std::string&>::convertToJava(const std::string& val) {
    return toJString(val);
}

template <>
template <>
inline jobject ToJavaConverter<const char *>::convertToJava(const char * val) {
    return toJString(val);
}

template <>
template <>
inline jobject ToJavaConverter<char *>::convertToJava(char * val) {
    return toJString(val);
}


template <typename DeclaredReturnType>
struct Actualizer {
    static typename Actualized<DeclaredReturnType>::type passThroughOrConvert(typename Actualized<DeclaredReturnType>::type returnValue, JniLocalReferenceScope& refs);
};

//
// Default implementation which just passes the value through unchanged.  Used for everything other than jobject/jlocal/jglobal/jarray/jobjectarray.
//
template <typename DeclaredReturnType>
inline typename Actualized<DeclaredReturnType>::type Actualizer<DeclaredReturnType>::passThroughOrConvert(typename Actualized<DeclaredReturnType>::type returnValue, JniLocalReferenceScope& refs) {
    return returnValue;
}

//
// A jglobal return value has been specified, convert the local reference jobject into a global
// reference jobject and return it.
//
template <>
inline jobject Actualizer<jglobal>::passThroughOrConvert(jobject returnValue, JniLocalReferenceScope& refs) {
	if (env()->IsSameObject(returnValue, nullptr)) {
		return nullptr;
	}
    jobject globalVal = env()->NewGlobalRef(returnValue);
    env()->DeleteLocalRef(returnValue);
    return globalVal;
}

//
// A jlocal (or jobject as they are the same) return value has been specified.  Unlike a global
// reference, it doesn't need to be converted however since it is a local object, when the JNI
// frame is popped, it will no longer be valid.  Pop the frame early and pass the reference
// through to get a new reference in the old frame to the same Java object.
//
template <>
inline jobject Actualizer<jlocal>::passThroughOrConvert(jobject returnValue, JniLocalReferenceScope& refs) {
	if (env()->IsSameObject(returnValue, nullptr)) {
		return nullptr;
	}
    return refs.releaseLocalRefs(returnValue);
}
template <>
inline jobjectArray Actualizer<jobjectArray>::passThroughOrConvert(jobjectArray returnValue, JniLocalReferenceScope& refs) {
	if (env()->IsSameObject(returnValue, nullptr)) {
		return nullptr;
	}
    return static_cast<jobjectArray>(refs.releaseLocalRefs(returnValue));
}
template <>
inline jarray Actualizer<jarray>::passThroughOrConvert(jarray returnValue, JniLocalReferenceScope& refs) {
	if (env()->IsSameObject(returnValue, nullptr)) {
		return nullptr;
	}
    return static_cast<jarray>(refs.releaseLocalRefs(returnValue));
}


} // namespace jni_pp
