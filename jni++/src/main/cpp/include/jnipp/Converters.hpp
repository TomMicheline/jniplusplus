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

template<typename CppType>
struct IsGlobalRef {
    static constexpr bool value = false;
};
template<>
struct IsGlobalRef<jglobal> {
    static constexpr bool value = true;
};

template<typename CppType>
struct JniSignature {
    std::string signature() { return "unknown"; }
    std::string typeName() { return "unknown"; }
};

template <typename DeclaredCppType>
struct JniTypeMapping {
    using actualCppType = DeclaredCppType;
    using jniType = DeclaredCppType;
};
template<> struct JniTypeMapping<jglobal> {
    using actualCppType = jobject;
    using jniType = jobject;
};

template<typename JavaType>
struct JavaToArray {
    typedef jarray type;
};

// Default implementations just cast to the other type
template <typename DeclaredCppType>
struct ToCppConverter {
    using cppType = typename JniTypeMapping<DeclaredCppType>::actualCppType;
    using jniType = typename JniTypeMapping<DeclaredCppType>::jniType;
    static cppType convertToCpp(jniType val) {
        return cppType(val);
    }
};
template <typename DeclaredCppType>
struct ToJavaConverter {
    using cppType = typename JniTypeMapping<DeclaredCppType>::actualCppType;
    using  jniType = typename JniTypeMapping<DeclaredCppType>::jniType;
    static jniType convertToJava(cppType value) {
        return jniType(value);
    }
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
    static void gather(vector<jvalue> &javaArgs, typename JniTypeMapping<ArgType>::actualCppType arg1, typename JniTypeMapping<Args>::actualCppType ...remainingArgs) {
        jvalue jarg1 = convertToJValue(ToJavaConverter<ArgType>::convertToJava(arg1));
        javaArgs.push_back(jarg1);
        GatherArguments<Args...>::gather(javaArgs, remainingArgs...);
    }
};

template <>
struct GatherArguments<> {
    static void gather(vector<jvalue> &javaArgs) {
    }
};

template <typename... Args>
struct GatherSignature;

template <typename ArgType, typename... Args>
struct GatherSignature<ArgType, Args...> {
    static void gather(std::stringstream& signatureBuffer, std::vector<std::string>& argumentTypes, int skipNum) {
        if (skipNum <= 0) {
            JniSignature<ArgType> signer;
            signatureBuffer << signer.signature();
            argumentTypes.push_back(signer.typeName());
        }
        GatherSignature<Args...>::gather(signatureBuffer, argumentTypes, skipNum - 1);
    }
};

template <>
struct GatherSignature<> {
    static void gather(std::stringstream& signatureBuffer, std::vector<std::string>& argumentTypes, int skipNum) {
    }
};

template<> inline std::string JniSignature<void>::signature() { return "V"; }
template<> inline std::string JniSignature<short>::signature() { return "S"; }
template<> inline std::string JniSignature<int>::signature() { return "I"; }
template<> inline std::string JniSignature<long>::signature() { return "J"; }

template<> inline std::string JniSignature<unsigned char>::signature() { return "B"; }
template<> inline std::string JniSignature<char>::signature() { return "C"; }

template<> inline std::string JniSignature<float>::signature() { return "F"; }
template<> inline std::string JniSignature<double>::signature() { return "D"; }

template<> inline std::string JniSignature<bool>::signature() { return "Z"; }

template<> inline std::string JniSignature<char *>::signature() { return "Ljava/lang/String;"; }
template<> inline std::string JniSignature<const char *>::signature() { return "Ljava/lang/String;"; }
template<> inline std::string JniSignature<std::string>::signature() { return "Ljava/lang/String;"; }
template<> inline std::string JniSignature<const std::string&>::signature() { return "Ljava/lang/String;"; }

template<> inline std::string JniSignature<jobject>::signature() { return "Ljava/lang/Object;"; }

template<> inline std::string JniSignature<void>::typeName() { return "void"; }
template<> inline std::string JniSignature<short>::typeName() { return "short"; }
template<> inline std::string JniSignature<int>::typeName() { return "int"; }
template<> inline std::string JniSignature<long>::typeName() { return "long"; }

template<> inline std::string JniSignature<unsigned char>::typeName() { return "byte"; }
template<> inline std::string JniSignature<char>::typeName() { return "char"; }

template<> inline std::string JniSignature<float>::typeName() { return "float"; }
template<> inline std::string JniSignature<double>::typeName() { return "double"; }

template<> inline std::string JniSignature<bool>::typeName() { return "boolean"; }

template<> inline std::string JniSignature<char *>::typeName() { return "java.lang.String"; }
template<> inline std::string JniSignature<const char *>::typeName() { return "java.lang.String"; }
template<> inline std::string JniSignature<std::string>::typeName() { return "java.lang.String"; }
template<> inline std::string JniSignature<const std::string&>::typeName() { return "java.lang.String"; }

template<> inline std::string JniSignature<jobject>::typeName() { return "java.lang.Object"; }

template<> struct JniTypeMapping<std::string> { 
    using jniType = jobject; 
    using actualCppType = std::string;
};
template<> struct JniTypeMapping<const char *> {
    using jniType = jobject;
    using actualCppType = const char *;
};
template<> struct JniTypeMapping<char *> {
    using jniType = jobject;
    using actualCppType = char *;
};
template<> struct JniTypeMapping<bool> {
    using jniType = jboolean;
    using actualCppType = bool;
};
template<> struct JniTypeMapping<unsigned char> {
    using jniType = jbyte;
    using actualCppType = unsigned char;
};
template<> struct JniTypeMapping<char> {
    using jniType = jchar;
    using actualCppType = char;
};
template<> struct JniTypeMapping<short> {
    using jniType = jshort;
    using actualCppType = short;
};
template<> struct JniTypeMapping<int> {
    using jniType = jint;
    using actualCppType = int;
};
template<> struct JniTypeMapping<long> {
    using jniType = jlong;
    using actualCppType = long;
};
template<> struct JniTypeMapping<long long> {
    using jniType = jlong;
    using actualCppType = long long;
};
template<> struct JniTypeMapping<float> {
    using jniType = jfloat;
    using actualCppType = float;
};
template<> struct JniTypeMapping<double> {
    using jniType = jdouble;
    using actualCppType = double;
};

template<> struct JavaToArray<jboolean> { typedef jbooleanArray type; };
template<> struct JavaToArray<jbyte> { typedef jbyteArray type; };
template<> struct JavaToArray<jchar> { typedef jcharArray type; };
template<> struct JavaToArray<jshort> { typedef jshortArray type; };
template<> struct JavaToArray<jint> { typedef jintArray type; };
template<> struct JavaToArray<jlong> { typedef jlongArray type; };
template<> struct JavaToArray<jfloat> { typedef jfloatArray type; };
template<> struct JavaToArray<jdouble> { typedef jdoubleArray type; };

template <>
inline std::string ToCppConverter<std::string>::convertToCpp(jobject val) {
	std::string result = jStringToString(static_cast<jstring>(val));
	env()->DeleteLocalRef(val);
	return result;
}
template <>
inline const char * ToCppConverter<const char *>::convertToCpp(jobject val) {
    char * result = jStringToCharPtr(static_cast<jstring>(val));
    env()->DeleteLocalRef(val);
    return result;
}
template <>
inline char * ToCppConverter<char *>::convertToCpp(jobject val) {
    char * result = jStringToCharPtr(static_cast<jstring>(val));
    env()->DeleteLocalRef(val);
    return result;
}
template <>
inline jchar ToJavaConverter<char>::convertToJava(char val) {
    return jchar(val);
}
template <>
inline jobject ToJavaConverter<std::string>::convertToJava(std::string val) {
    return toJString(val);
}
template <>
inline jobject ToJavaConverter<char *>::convertToJava(char * val) {
    return toJString(val);
}
template <>
inline jobject ToJavaConverter<const char *>::convertToJava(const char * val) {
    return toJString(val);
}


// Default implementation which just passes the value through unchanged.  Used for everything other than jobject/jlocal/jglobal/jarray/jobjectarray.
template <typename ActualReturnType, bool global>
struct JvmObjectPassThrough {
    static ActualReturnType pass(ActualReturnType returnValue, JniLocalReferenceScope& refs) {
        return returnValue;
    }
};

//
// If a global return value has been specified, convert the local reference jobject into a global
// reference jobject and return it.
//
// Otherwise, unlike a global reference, it doesn't need to be converted however since it is a local
// object, when the JNI frame is popped, it will no longer be valid.  Pop the frame early and pass the
// reference through to get a new reference in the old frame to the same Java object.
//
template<bool global>
struct JvmObjectPassThrough<jobject, global> {
    static jobject pass(jobject returnValue, JniLocalReferenceScope &refs) {
        if (env()->IsSameObject(returnValue, nullptr)) {
            return nullptr;
        }
        if (global) {
            jobject globalVal = env()->NewGlobalRef(returnValue);
            env()->DeleteLocalRef(returnValue);
            return globalVal;
        } else {
            return refs.releaseLocalRefs(returnValue);
        }
    }
};

template<bool global>
struct JvmObjectPassThrough<jobjectArray, global> {
    static jobjectArray pass(jobjectArray returnValue, JniLocalReferenceScope &refs) {
        if (env()->IsSameObject(returnValue, nullptr)) {
            return nullptr;
        }
        return static_cast<jobjectArray>(refs.releaseLocalRefs(returnValue));
    }
};

template<bool global>
struct JvmObjectPassThrough<jarray, global> {
    static jarray pass(jarray returnValue, JniLocalReferenceScope &refs) {
        if (env()->IsSameObject(returnValue, nullptr)) {
            return nullptr;
        }
        return static_cast<jarray>(refs.releaseLocalRefs(returnValue));
    }
};



} // namespace jni_pp
