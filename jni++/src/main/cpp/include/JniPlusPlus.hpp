//
// JniPlusPlus.hpp
// jni++
//
// Created by Thomas Micheline Jun 22, 2013.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
// Copyright © 2023 VSee Lab, Inc. All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

/// @file JniPlusPlus.hpp
/// @author Tom Micheline <tom@tmich.dev>
/// @brief Main header file for jni++
///
/// Contains the class used to define and invoke JVM methods, constructors and interact with arrays.  Also
/// the most commonly used public functions are defined here.  Other, private or less frequently used functions
/// are in files under jnipp.

#pragma once

#include <jni.h>

#include <memory>
#include <cstring>
#include <mutex>
#include <typeinfo>
#include <typeindex>
#include <map>
#include <sstream>
#include <iostream>

#include "jnipp/Utilities.hpp"
#include "jnipp/Converters.hpp"
#include "jnipp/InvokersLowLevel.hpp"
#include "jnipp/InvokersHighLevel.hpp"
#include "jnipp/References.hpp"
#include "jnipp/Loggers.hpp"

namespace jni_pp {

/// @brief Method pure-virtual base class.
///
/// This class forms the base class for all method invocation classes.  It can not be used directly.
/// @tparam ReturnType The C++ returned by the invoked method
/// @tparam Args Variadic list of method parameter types.  In some cases (for example an instance method) the
/// first type will be the jobject reference to the target java object.
template <typename ReturnType, typename... Args>
class Method {
public:
	virtual ~Method() = default;

    /// @brief Take the passed in JVM class name and return the full class name.
    ///
    /// Applies rules such as applying a default package and/or swig mappings to determine the canonical class name.
    /// @return The class name as a string
    const std::string& getClassName() {
        call_once(runCNOnce, [&]{
            cn = calculateClassName(givenClassName);
        });
        return cn;
    }

    /// @brief Lookup the MethodInfo for this Method.
    ///
    /// The actual lookup is only performed the first time and cached.  Each instance caches the results in the Mehtod
    /// object but, if you have more than one Method object that refers to the same JVM method, the results are also
    /// cached globally.  The returned structure includes the jclass and jmethodID for the method.
    /// @return Pointer to MethodInfo struct.  Will never be nullptr, currently aborts if method or class cannot be found.
	MethodInfo* getMethodInfo() {
		call_once(runMIOnce, [&]{
            methodInfo = getMethodInfoInternal();
        });
		return methodInfo;
	}

    /// @brief Function call operator.
    ///
    /// Performs the common logic for JVM method invocation but uses the pure-virtual invoke() function to perform the
    /// type specific invocation logic.
    /// @param args The arguments to pass to the JVM method
    /// @return The value returned by the JVM method, converted to the ReturnType C++ type.
	typename JniTypeMapping<ReturnType>::actualCppType operator()(typename JniTypeMapping<Args>::actualCppType ...args);

protected:

    /// @brief Subclass implemented function to lookup the MethodInfo.
    ///
    /// Does not need to worry about thread safety, that is handled by getMethodInfo() which is the only place
    /// this should be called.
    /// @return Pointer to MethodInfo struct which contains the jclass and jmethodID.
    virtual MethodInfo* getMethodInfoInternal() = 0;

    /// @brief Method constructor only visible to implementing classes.
    /// @param className Name of the JVM class
    /// @param methodName Name of the JVM method
    /// @param signature JNI signature which can be empty
    /// @param isStatic Is this a static method
    /// @param numReservedParameters How many parameters are used by the implementation and not passed to the JVM method.  Generally, 0 or 1 depending
    /// on if there is an instance object being passed in as the first parameter.
    Method(const std::string& className, const std::string& methodName, const std::string& signature, const std::string& returnSignature, int numReservedParameters) :
            givenClassName(className), methodName(methodName), methodSignature(signature), numReservedParameters(numReservedParameters)
    {
        std::stringstream sigbuf{};
        std::vector<std::string> returnType;

        sigbuf << "(";
        GatherSignature<Args...>::gather(sigbuf, parameterJniTypeNames, numReservedParameters);
        sigbuf << ")";
        if (returnSignature.empty()) {
            GatherSignature<ReturnType>::gather(sigbuf, returnType, 0);
        } else {
            sigbuf << returnSignature;
        }
        computedSignature = sigbuf.str();
        if (methodSignature.empty()) {
            methodSignature = computedSignature;
        }

        getLogger()->debug( "Signature for " + className + "." + methodName + ": " + computedSignature );
    }

    /// @brief Invoke the refered to JVM method with the given arguments and return the converted results.
    ///
    ///
    /// @param javaArgs
    /// @param refs
    /// @return
    virtual typename JniTypeMapping<ReturnType>::actualCppType invoke(vector<jvalue>& javaArgs, JniLocalReferenceScope& refs) = 0;

private:
    static const std::string calculateClassName(const std::string& given) {
        // The class name passed in from the base class may already have the swig package prepended.  If it does,
        // just return it.  Otherwise, add the base package to the front (empty string by default).
        auto& swig = getSwigPackage();
        auto& base = getPackageBase();

        if (!base.empty() && (swig.empty() || given.rfind(swig, 0) == std::string::npos)) {
            return base + "." + given;
        } else {
            return given;
        }
    }

protected:
    const std::string givenClassName;
    const std::string methodName;

    std::string methodSignature;
    std::string computedSignature;
    std::vector<std::string> parameterJniTypeNames;

    int numParameters;
    int numReservedParameters;

private:
    std::string cn;
	MethodInfo *methodInfo;
	std::once_flag runMIOnce, runCNOnce;
};



template <typename ReturnType, typename... Args>
class StaticMethod : public Method<ReturnType, Args...> {
public:

    typedef Method<ReturnType, Args...> Base;
    using Base::getClassName, Base::getMethodInfo, Base::methodName, Base::methodSignature, Base::numParameters;

	StaticMethod(const std::string& className, const std::string& methodName, const std::string& signature = "") : Method<ReturnType, Args...>(className, methodName, signature, "", 0) {}
	virtual ~StaticMethod() {}

protected:
    MethodInfo* getMethodInfoInternal() {
        return  jni_pp::getMethodInfo(getClassName(), methodName, methodSignature, true, numParameters);
    }

    typename JniTypeMapping<ReturnType>::actualCppType invoke(vector<jvalue>& javaArgs, JniLocalReferenceScope& refs);
};



template <typename ReturnType, typename... Args>
class InstanceMethod :  public Method<ReturnType, jobject, Args...> {
public:

    typedef Method<ReturnType, jobject, Args...> Base;
    using Base::getClassName, Base::getMethodInfo, Base::methodName, Base::methodSignature, Base::numParameters;

	InstanceMethod(const std::string& className, const std::string& methodName, const std::string& signature = "") :
        Method<ReturnType, jobject, Args...>(className, methodName, signature, "", 1)
        {}
	virtual ~InstanceMethod() {}

protected:
    MethodInfo* getMethodInfoInternal() {
        return  jni_pp::getMethodInfo(getClassName(), methodName, methodSignature, false, numParameters);
    }

    typename JniTypeMapping<ReturnType>::actualCppType invoke(vector<jvalue>& javaArgs, JniLocalReferenceScope& refs);
};

template <typename ReturnType, typename... Args>
class SingletonMethod : public Method<ReturnType, Args...> {
public:

    typedef Method<ReturnType, Args...> Base;
    using Base::getClassName, Base::getMethodInfo, Base::methodName, Base::methodSignature, Base::numParameters;

    SingletonMethod(const std::string& className, const std::string& methodName, const std::string& signature = "") :
        Method<ReturnType, Args...>(className, methodName, signature, "", 0)
        {}
    virtual ~SingletonMethod() {}

protected:
    MethodInfo* getMethodInfoInternal() {
        return  jni_pp::getMethodInfo(getClassName(), methodName, methodSignature, false, numParameters);
    }

    typename JniTypeMapping<ReturnType>::actualCppType invoke(vector<jvalue>& javaArgs, JniLocalReferenceScope& refs);
};

template <typename ReturnType, typename... Args>
class Constructor :  public Method<ReturnType, Args...> {
public:

	typedef Method<ReturnType, Args...> Base;
    using Base::getClassName, Base::getMethodInfo, Base::methodName, Base::methodSignature, Base::numParameters;

	Constructor(const std::string& className, const std::string& signature = "") : Method<ReturnType, Args...>(className, "<init>", signature, "V", 0) {}
	virtual ~Constructor() {}

protected:
    MethodInfo* getMethodInfoInternal() {
        return  jni_pp::getMethodInfo(getClassName(), methodName, methodSignature, false, numParameters);
    }

    jobject invoke(vector<jvalue>& javaArgs, JniLocalReferenceScope& refs);
};

template <typename FieldType>
class Field {
public:
    
    FieldInfo* getFieldInfo() {
        call_once(runOnce, [&]{
            fieldInfo = jni_pp::getFieldInfo(className, fieldName, fieldSignature, isStatic);
        });
        return fieldInfo;
    }
    virtual ~Field() {}

protected:

    Field(const std::string& className, const std::string& fieldName, const std::string& fieldSignature, bool isStatic) : className(className), fieldName(fieldName), fieldSignature(fieldSignature), isStatic(isStatic) {
    }

    const std::string className;
    const std::string fieldName;
    const std::string fieldSignature;
    bool isStatic;

private:
    FieldInfo *fieldInfo;
    std::once_flag runOnce;
};

template <typename FieldType>
class InstanceField : public Field<FieldType> {
public:

    typedef Field<FieldType> Base;
    using Base::getFieldInfo;

    InstanceField(const std::string& className, const std::string& fieldName, const std::string& fieldSignature = "") : Field<FieldType>(className, fieldName, fieldSignature, false) {
    }
    ~InstanceField() {}

    typename JniTypeMapping<FieldType>::actualCppType get(jobject object);
    void set(jobject object, typename JniTypeMapping<FieldType>::actualCppType value);
};

template <typename FieldType>
class SingletonField : public Field<FieldType> {
public:

    typedef Field<FieldType> Base;
    using Base::getFieldInfo;
    using Base::className;

    SingletonField(const std::string& className, const std::string& fieldName, const std::string& fieldSignature = "") :
        Field<FieldType>(className, fieldName, fieldSignature, false)
        {}
    ~SingletonField() {}

    typename JniTypeMapping<FieldType>::actualCppType get();
    void set(typename JniTypeMapping<FieldType>::actualCppType value);
};

template <typename FieldType>
class StaticField : public Field<FieldType> {
public:

    typedef Field<FieldType> Base;
    using Base::getFieldInfo;

    StaticField(const std::string& className, const std::string& fieldName, const std::string& fieldSignature = "") : Field<FieldType>(className, fieldName, fieldSignature, true) {
    }
    ~StaticField() {}

    typename JniTypeMapping<FieldType>::actualCppType get();
    void set(typename JniTypeMapping<FieldType>::actualCppType value);
};


template <typename CppElementType>
class PrimitiveArray {
public:
    static_assert(std::is_arithmetic<CppElementType>::value, "PrimitiveArrays only supports primitive Java types.  Use ObjectArray for non-primitives.");

    using JavaType = typename JniTypeMapping<CppElementType>::jniType;
    typedef typename JavaToArray<JavaType>::type ArrayType;

    template <typename ReturnType>
    static ArrayType create(int size);

    template <typename ReturnType>
    static ArrayType convert(CppElementType cppArray[], int size);

    template <typename ReturnType>
    static ArrayType convert(CppElementType cppArray[], int start, int size);

    static void set(ArrayType javaArray, CppElementType cppArray[], int len);
    static void set(ArrayType javaArray, CppElementType cppArray[], int start, int len);

    static CppElementType* get(ArrayType array, bool* isCopy = nullptr);
    static void release(ArrayType array, CppElementType* arrayPointer);

    static CppElementType* getCritical(ArrayType array, bool* isCopy = nullptr);
    static void releaseCritical(ArrayType array, CppElementType* arrayPointer);

    static int size(ArrayType array);
};

template <typename CppElementType>
class ObjectArray {
public:

    ObjectArray(const std::string& className) : className(className) {
    }

    jclass getClass() {
        call_once(runOnce, [&]{
            class_ = jni_pp::getClass(className);
        });
        return class_;
    }

    int size(jobjectArray array) { return env()->GetArrayLength(array); }

    template <typename ReturnType>
    jobjectArray create(int size, jobject initialElement = nullptr);

    typename JniTypeMapping<CppElementType>::actualCppType get(jobjectArray array, int index);

    void set(jobjectArray array, int index, typename JniTypeMapping<CppElementType>::actualCppType value);

private:
    const std::string className;

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

template <typename ReturnType, typename... Args>
typename JniTypeMapping<ReturnType>::actualCppType StaticMethod<ReturnType, Args...>::invoke(vector<jvalue>& javaArgs, JniLocalReferenceScope& refs) {
    return HighLevelInvoker<ReturnType>::invoke(getMethodInfo()->class_, getMethodInfo()->methodID, javaArgs, refs);
}

template <typename ReturnType, typename... Args>
typename JniTypeMapping<ReturnType>::actualCppType InstanceMethod<ReturnType, Args...>::invoke(vector<jvalue>& javaArgs, JniLocalReferenceScope& refs) {
    jvalue jv = javaArgs.front();
    jobject object = jv.l;
    javaArgs.erase(javaArgs.begin(), javaArgs.begin() + 1);
    return HighLevelInvoker<ReturnType>::invoke(object, getMethodInfo()->methodID, javaArgs, refs);
}

template <typename ReturnType, typename... Args>
typename JniTypeMapping<ReturnType>::actualCppType SingletonMethod<ReturnType, Args...>::invoke(vector<jvalue>& javaArgs, JniLocalReferenceScope& refs) {
    // Don't ever cache the singleton object.  It might change...
    jobject object = getSingletonObject(getClassName());
    if (!object) {
        throw std::runtime_error(std::string("Singleton ") + getClassName() + " not currently available when referenced in SingletonMethod");
    }
    return HighLevelInvoker<ReturnType>::invoke(object, getMethodInfo()->methodID, javaArgs, refs);
}

template <typename ReturnType, typename... Args>
jobject Constructor<ReturnType, Args...>::invoke(vector<jvalue>& javaArgs, JniLocalReferenceScope& refs) {
    using actualReturnType = typename JniTypeMapping<ReturnType>::actualCppType;
    return JvmObjectPassThrough<actualReturnType, IsGlobalRef<ReturnType>::value>::pass(env()->NewObjectA(getMethodInfo()->class_, getMethodInfo()->methodID, javaArgs.data()), refs);
}


template <typename ReturnType, typename... Args>
typename JniTypeMapping<ReturnType>::actualCppType Method<ReturnType, Args...>::operator ()(typename JniTypeMapping<Args>::actualCppType ...args) {
    numParameters = sizeof...(args) - numReservedParameters;
    vector<jvalue> javaArgs;
    javaArgs.reserve(numParameters + numReservedParameters);

    //
    // Use JniLocalReferenceScope to push a new JNI frame so that all local references will be cleaned up.  If ReturnType is jobject,
    // however, we need to pass in the returned java object to get a new reference in the old frame.  Pass refs into the
    // subclass invoke, which passes it to the HighLeverInvoker, which (if this isn't a void return Method) passes it into
    // the JvmObjectPassThrough which will, for a jobject, pop the frame early and pass the jobject return value to get a reference
    // that will still be valid in the calling frame.
    //
    JniLocalReferenceScope refs(numParameters + numReservedParameters + 1);
    GatherArguments<Args...>::gather(javaArgs, args...);
    return invoke(javaArgs, refs);
}

template <typename FieldType>
typename JniTypeMapping<FieldType>::actualCppType InstanceField<FieldType>::get(jobject object) {
    JniLocalReferenceScope refs;
    return HighLevelAccessor<FieldType>::get(object, getFieldInfo()->fieldID, refs);
}

template <typename FieldType>
void InstanceField<FieldType>::set(jobject object, typename JniTypeMapping<FieldType>::actualCppType value) {
    JniLocalReferenceScope refs;
    HighLevelAccessor<FieldType>::set(object, getFieldInfo()->fieldID, value);
}

template <typename FieldType>
typename JniTypeMapping<FieldType>::actualCppType SingletonField<FieldType>::get() {
    JniLocalReferenceScope refs;
    // Don't ever cache the singleton object.  It might change...
    jobject object = getSingletonObject(className);
    if (!object) throw new std::runtime_error(std::string("Singleton ") + className + " not available when referenced by SingletonField.");
    return HighLevelAccessor<FieldType>::get(object, getFieldInfo()->fieldID, refs);
}

template <typename FieldType>
void SingletonField<FieldType>::set(typename JniTypeMapping<FieldType>::actualCppType value) {
    JniLocalReferenceScope refs;
    jobject object = getSingletonObject(className);
    if (!object) throw new std::runtime_error(std::string("Singleton ") + className + " not available when referenced by SingletonField.");
    HighLevelAccessor<FieldType>::set(object, getFieldInfo()->fieldID, value);
}

template <typename FieldType>
typename JniTypeMapping<FieldType>::actualCppType StaticField<FieldType>::get() {
    JniLocalReferenceScope refs;
    return HighLevelAccessor<FieldType>::get(getFieldInfo()->class_, getFieldInfo()->fieldID, refs);
}

template <typename FieldType>
void StaticField<FieldType>::set(typename JniTypeMapping<FieldType>::actualCppType value) {
    JniLocalReferenceScope refs;
    HighLevelAccessor<FieldType>::set(getFieldInfo()->class_, getFieldInfo()->fieldID, value);
}

template <typename CppElementType>
template <typename ReturnType>
typename PrimitiveArray<CppElementType>::ArrayType PrimitiveArray<CppElementType>::create(int size) {
    JniLocalReferenceScope refs;
    ArrayType array = LowLevelAccessor<JavaType>::createJavaArray(size);
    return static_cast<ArrayType>(JvmObjectPassThrough<ReturnType, false>::pass(array, refs));
}

template <typename CppElementType>
template <typename ReturnType>
jobjectArray ObjectArray<CppElementType>::create(int size, jobject initialElement) {
    JniLocalReferenceScope refs;
    jarray array = env()->NewObjectArray(size, getClass(), initialElement);
    return static_cast<jobjectArray>(JvmObjectPassThrough<ReturnType, false>::pass(array, refs));
}


template <typename CppElementType>
template <typename ReturnType>
typename PrimitiveArray<CppElementType>::ArrayType PrimitiveArray<CppElementType>::convert(CppElementType cppArray[], int size) {
    ArrayType javaArray = create<ReturnType>(size);
    set(javaArray, cppArray, size);
    return javaArray;
}


template <typename CppElementType>
template <typename ReturnType>
typename PrimitiveArray<CppElementType>::ArrayType PrimitiveArray<CppElementType>::convert(CppElementType cppArray[], int start, int size) {
    ArrayType javaArray = create<ReturnType>(size);
    set(javaArray, cppArray, start, size);
    return javaArray;
}

template <typename CppElementType>
void PrimitiveArray<CppElementType>::set(ArrayType javaArray, CppElementType cppArray[], int start, int size) {
    JniLocalReferenceScope refs;
    LowLevelAccessor<JavaType>::setElements(javaArray, cppArray, start, size);
}

template <typename CppElementType>
void PrimitiveArray<CppElementType>::set(ArrayType javaArray, CppElementType cppArray[], int len) {
    set(javaArray, cppArray, 0, len);
}

template <typename CppElementType>
CppElementType* PrimitiveArray<CppElementType>::get(ArrayType array, bool* isCopy) {
    JniLocalReferenceScope refs;
    jboolean jniIsCopy;
    CppElementType* arrayPointer = (CppElementType*) LowLevelAccessor<JavaType>::getElements(array, &jniIsCopy);
    if (isCopy) {
        *isCopy = jniIsCopy == JNI_TRUE;
    }
    return arrayPointer;
}

template <typename CppElementType>
void PrimitiveArray<CppElementType>::release(ArrayType array, CppElementType* arrayPointer) {
    LowLevelAccessor<JavaType>::releaseElements(array, arrayPointer);
}

template <typename CppElementType>
CppElementType* PrimitiveArray<CppElementType>::getCritical(ArrayType array, bool* isCopy) {
    jboolean jniIsCopy;
    CppElementType* arrayPointer = (CppElementType*) env()->GetPrimitiveArrayCritical(array, &jniIsCopy);
    if (isCopy) {
        *isCopy = jniIsCopy == JNI_TRUE;
    }
    return arrayPointer;
}

template <typename CppElementType>
void PrimitiveArray<CppElementType>::releaseCritical(ArrayType array, CppElementType* arrayPointer) {
    env()->ReleasePrimitiveArrayCritical(array, arrayPointer, 0);
}

template <typename CppElementType>
int PrimitiveArray<CppElementType>::size(ArrayType array) {
    return env()->GetArrayLength(array);
}

template <typename CppElementType>
typename JniTypeMapping<CppElementType>::actualCppType ObjectArray<CppElementType>::get(jobjectArray array, int index) {
    using actualReturnType = typename JniTypeMapping<CppElementType>::actualCppType;
    JniLocalReferenceScope refs;
    jobject javaReturn =  env()->GetObjectArrayElement(array, index);
    actualReturnType value = ToCppConverter<actualReturnType>::convertToCpp(javaReturn);
    return JvmObjectPassThrough<actualReturnType, IsGlobalRef<CppElementType>::value>::pass(value, refs);
}

template <typename CppElementType>
void ObjectArray<CppElementType>::set(jobjectArray array, int index, typename JniTypeMapping<CppElementType>::actualCppType  value) {
    JniLocalReferenceScope refs;
    jobject javaValue = convertToJValue(value);
    env()->SetObjectArrayElement(array, index, javaValue);
}

} // namespace jni_pp

