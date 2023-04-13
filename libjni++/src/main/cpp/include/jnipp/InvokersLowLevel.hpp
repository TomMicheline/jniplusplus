//
// LowLevelInvokers.cpp
// libjni++
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


namespace jni_pp {

template <typename JavaReturnType>
struct LowLevelInvoker {
    // Static and Instance method invokers
    static JavaReturnType invoke(jclass class_, jmethodID methodID, vector <jvalue> &args);
    static JavaReturnType invoke(jobject object, jmethodID methodID, vector <jvalue> &args);
};    

template <typename JavaType>
struct LowLevelAccessor {
    // Static and Instance field getters
    static JavaType get(jclass  class_, jfieldID);
    static JavaType get(jobject object, jfieldID);

    // Static and Instance field setters
    static void set(jclass  class_, jfieldID, JavaType value);
    static void set(jobject object, jfieldID, JavaType value);

    // Primitive arrays have specialized jarray types based upon JavaType
    typedef typename JavaToArray<JavaType>::type ArrayType;

    // Primitive array create
    static ArrayType createJavaArray(int size);
    
    // Primitive array get and release
    static JavaType* getElements(jarray array, jboolean* isCopy);
    static void releaseElements(jarray array, JavaType* elements);

    // Primitive array set
    static void setElements(ArrayType javaArray, JavaType cppArray[], int start, int len);
};



/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
//////////
//////////
//////////  Implementation details
//////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////



//
// Low level static method invokers.
//

template<>
inline void LowLevelInvoker<void>::invoke(jclass class_, jmethodID methodID, vector<jvalue>& args) {
	env()->CallStaticVoidMethodA(class_, methodID, args.data());
}

template<>
inline jboolean LowLevelInvoker<jboolean>::invoke(jclass class_, jmethodID methodID, vector<jvalue>& args) {
	return env()->CallStaticBooleanMethodA(class_, methodID, args.data());
}

template<>
inline jbyte LowLevelInvoker<jbyte>::invoke(jclass class_, jmethodID methodID, vector<jvalue>& args) {
	return env()->CallStaticByteMethodA(class_, methodID, args.data());
}

template<>
inline jchar LowLevelInvoker<jchar>::invoke(jclass class_, jmethodID methodID, vector<jvalue>& args) {
	return env()->CallStaticCharMethodA(class_, methodID, args.data());
}

template<>
inline jshort LowLevelInvoker<jshort>::invoke(jclass class_, jmethodID methodID, vector<jvalue>& args) {
	return env()->CallStaticShortMethodA(class_, methodID, args.data());
}

template<>
inline jint LowLevelInvoker<jint>::invoke(jclass class_, jmethodID methodID, vector<jvalue>& args) {
	return env()->CallStaticIntMethodA(class_, methodID, args.data());
}

template<>
inline jlong LowLevelInvoker<jlong>::invoke(jclass class_, jmethodID methodID, vector<jvalue>& args) {
	return env()->CallStaticLongMethodA(class_, methodID, args.data());
}

template<>
inline jfloat LowLevelInvoker<jfloat>::invoke(jclass class_, jmethodID methodID, vector<jvalue>& args) {
	return env()->CallStaticFloatMethodA(class_, methodID, args.data());
}

template<>
inline jdouble LowLevelInvoker<jdouble>::invoke(jclass class_, jmethodID methodID, vector<jvalue>& args) {
	return env()->CallStaticDoubleMethodA(class_, methodID, args.data());
}

template<>
inline jobject LowLevelInvoker<jobject>::invoke(jclass class_, jmethodID methodID, vector<jvalue>& args) {
    return env()->CallStaticObjectMethodA(class_, methodID, args.data());
}

template<>
inline jarray LowLevelInvoker<jarray>::invoke(jclass class_, jmethodID methodID, vector<jvalue>& args) {
    return static_cast<jarray>(env()->CallStaticObjectMethodA(class_, methodID, args.data()));
}

template<>
inline jobjectArray LowLevelInvoker<jobjectArray>::invoke(jclass class_, jmethodID methodID, vector<jvalue>& args) {
    return static_cast<jobjectArray>(env()->CallStaticObjectMethodA(class_, methodID, args.data()));
}


//
// Instance mehtod invokers
//

template<>
inline void LowLevelInvoker<void>::invoke(jobject object, jmethodID methodID, vector<jvalue>& args) {
	env()->CallVoidMethodA(object, methodID, args.data());
}

template<>
inline jboolean LowLevelInvoker<jboolean>::invoke(jobject object, jmethodID methodID, vector<jvalue>& args) {
	return env()->CallBooleanMethodA(object, methodID, args.data());
}

template<>
inline jbyte LowLevelInvoker<jbyte>::invoke(jobject object, jmethodID methodID, vector<jvalue>& args) {
	return env()->CallByteMethodA(object, methodID, args.data());
}

template<>
inline jchar LowLevelInvoker<jchar>::invoke(jobject object, jmethodID methodID, vector<jvalue>& args) {
	return env()->CallCharMethodA(object, methodID, args.data());
}

template<>
inline jshort LowLevelInvoker<jshort>::invoke(jobject object, jmethodID methodID, vector<jvalue>& args) {
	return env()->CallShortMethodA(object, methodID, args.data());
}

template<>
inline jint LowLevelInvoker<jint>::invoke(jobject object, jmethodID methodID, vector<jvalue>& args) {
	return env()->CallIntMethodA(object, methodID, args.data());
}

template<>
inline jlong LowLevelInvoker<jlong>::invoke(jobject object, jmethodID methodID, vector<jvalue>& args) {
	return env()->CallLongMethodA(object, methodID, args.data());
}

template<>
inline jfloat LowLevelInvoker<jfloat>::invoke(jobject object, jmethodID methodID, vector<jvalue>& args) {
	return env()->CallFloatMethodA(object, methodID, args.data());
}

template<>
inline jdouble LowLevelInvoker<jdouble>::invoke(jobject object, jmethodID methodID, vector<jvalue>& args) {
	return env()->CallDoubleMethodA(object, methodID, args.data());
}

template<>
inline jobject LowLevelInvoker<jobject>::invoke(jobject object, jmethodID methodID, vector<jvalue>& args) {
    return env()->CallObjectMethodA(object, methodID, args.data());
}

template<>
inline jarray LowLevelInvoker<jarray>::invoke(jobject object, jmethodID methodID, vector<jvalue>& args) {
    return static_cast<jarray>(env()->CallObjectMethodA(object, methodID, args.data()));
}

template<>
inline jobjectArray LowLevelInvoker<jobjectArray>::invoke(jobject object, jmethodID methodID, vector<jvalue>& args) {
    return static_cast<jobjectArray>(env()->CallObjectMethodA(object, methodID, args.data()));
}


//
// Static field getters.
//

template<>
inline jboolean LowLevelAccessor<jboolean>::get(jclass class_, jfieldID fieldID) {
    return env()->GetStaticBooleanField(class_, fieldID);
}

template<>
inline jbyte LowLevelAccessor<jbyte>::get(jclass class_, jfieldID fieldID) {
    return env()->GetStaticByteField(class_, fieldID);
}

template<>
inline jchar LowLevelAccessor<jchar>::get(jclass class_, jfieldID fieldID) {
    return env()->GetStaticCharField(class_, fieldID);
}

template<>
inline jshort LowLevelAccessor<jshort>::get(jclass class_, jfieldID fieldID) {
    return env()->GetStaticShortField(class_, fieldID);
}

template<>
inline jint LowLevelAccessor<jint>::get(jclass class_, jfieldID fieldID) {
    return env()->GetStaticIntField(class_, fieldID);
}

template<>
inline jlong LowLevelAccessor<jlong>::get(jclass class_, jfieldID fieldID) {
    return env()->GetStaticLongField(class_, fieldID);
}

template<>
inline jfloat LowLevelAccessor<jfloat>::get(jclass class_, jfieldID fieldID) {
    return env()->GetStaticFloatField(class_, fieldID);
}

template<>
inline jdouble LowLevelAccessor<jdouble>::get(jclass class_, jfieldID fieldID) {
    return env()->GetStaticDoubleField(class_, fieldID);
}

template<>
inline jobject LowLevelAccessor<jobject>::get(jclass class_, jfieldID fieldID) {
    return env()->GetStaticObjectField(class_, fieldID);
}

template<>
inline jarray LowLevelAccessor<jarray>::get(jclass class_, jfieldID fieldID) {
    return static_cast<jarray>(env()->GetStaticObjectField(class_, fieldID));
}

template<>
inline jobjectArray LowLevelAccessor<jobjectArray>::get(jclass class_, jfieldID fieldID) {
    return static_cast<jobjectArray>(env()->GetStaticObjectField(class_, fieldID));
}


//
// Instance field getters
//

template<>
inline jboolean LowLevelAccessor<jboolean>::get(jobject object, jfieldID fieldID) {
    return env()->GetBooleanField(object, fieldID);
}

template<>
inline jbyte LowLevelAccessor<jbyte>::get(jobject object, jfieldID fieldID) {
    return env()->GetByteField(object, fieldID);
}

template<>
inline jchar LowLevelAccessor<jchar>::get(jobject object, jfieldID fieldID) {
    return env()->GetCharField(object, fieldID);
}

template<>
inline jshort LowLevelAccessor<jshort>::get(jobject object, jfieldID fieldID) {
    return env()->GetShortField(object, fieldID);
}

template<>
inline jint LowLevelAccessor<jint>::get(jobject object, jfieldID fieldID) {
    return env()->GetIntField(object, fieldID);
}

template<>
inline jlong LowLevelAccessor<jlong>::get(jobject object, jfieldID fieldID) {
    return env()->GetLongField(object, fieldID);
}

template<>
inline jfloat LowLevelAccessor<jfloat>::get(jobject object, jfieldID fieldID) {
    return env()->GetFloatField(object, fieldID);
}

template<>
inline jdouble LowLevelAccessor<jdouble>::get(jobject object, jfieldID fieldID) {
    return env()->GetDoubleField(object, fieldID);
}

template<>
inline jobject LowLevelAccessor<jobject>::get(jobject object, jfieldID fieldID) {
    return env()->GetObjectField(object, fieldID);
}

template<>
inline jarray LowLevelAccessor<jarray>::get(jobject object, jfieldID fieldID) {
    return static_cast<jarray>(env()->GetObjectField(object, fieldID));
}

template<>
inline jobjectArray LowLevelAccessor<jobjectArray>::get(jobject object, jfieldID fieldID) {
    return static_cast<jobjectArray>(env()->GetObjectField(object, fieldID));
}

//
// Static field setters
//

template<>
inline void LowLevelAccessor<jboolean>::set(jclass class_, jfieldID fieldID, jboolean value) {
    env()->SetStaticBooleanField(class_, fieldID, value);
}

template<>
inline void LowLevelAccessor<jbyte>::set(jclass class_, jfieldID fieldID, jbyte value) {
    env()->SetStaticByteField(class_, fieldID, value);
}

template<>
inline void LowLevelAccessor<jchar>::set(jclass class_, jfieldID fieldID, jchar value) {
    env()->SetStaticCharField(class_, fieldID, value);
}

template<>
inline void LowLevelAccessor<jshort>::set(jclass class_, jfieldID fieldID, jshort value) {
    env()->SetStaticShortField(class_, fieldID, value);
}

template<>
inline void LowLevelAccessor<jint>::set(jclass class_, jfieldID fieldID, jint value) {
    env()->SetStaticIntField(class_, fieldID, value);
}

template<>
inline void LowLevelAccessor<jlong>::set(jclass class_, jfieldID fieldID, jlong value) {
    env()->SetStaticLongField(class_, fieldID, value);
}

template<>
inline void LowLevelAccessor<jfloat>::set(jclass class_, jfieldID fieldID, jfloat value) {
    env()->SetStaticFloatField(class_, fieldID, value);
}

template<>
inline void LowLevelAccessor<jdouble>::set(jclass class_, jfieldID fieldID, jdouble value) {
    env()->SetStaticDoubleField(class_, fieldID, value);
}

template<>
inline void LowLevelAccessor<jobject>::set(jclass class_, jfieldID fieldID, jobject value) {
    env()->SetStaticObjectField(class_, fieldID, value);
}

template<>
inline void LowLevelAccessor<jarray>::set(jclass class_, jfieldID fieldID, jarray value) {
    env()->SetStaticObjectField(class_, fieldID, static_cast<jobject>(value));
}

template<>
inline void LowLevelAccessor<jobjectArray>::set(jclass class_, jfieldID fieldID, jobjectArray value) {
    env()->SetStaticObjectField(class_, fieldID, static_cast<jobject>(value));
}


//
// Instance field setters
//

template<>
inline void LowLevelAccessor<jboolean>::set(jobject object, jfieldID fieldID, jboolean value) {
    env()->SetBooleanField(object, fieldID, value);
}

template<>
inline void LowLevelAccessor<jbyte>::set(jobject object, jfieldID fieldID, jbyte value) {
    env()->SetByteField(object, fieldID, value);
}

template<>
inline void LowLevelAccessor<jchar>::set(jobject object, jfieldID fieldID, jchar value) {
    env()->SetCharField(object, fieldID, value);
}

template<>
inline void LowLevelAccessor<jshort>::set(jobject object, jfieldID fieldID, jshort value) {
    env()->SetShortField(object, fieldID, value);
}

template<>
inline void LowLevelAccessor<jint>::set(jobject object, jfieldID fieldID, jint value) {
    env()->SetIntField(object, fieldID, value);
}

template<>
inline void LowLevelAccessor<jlong>::set(jobject object, jfieldID fieldID, jlong value) {
    env()->SetLongField(object, fieldID, value);
}

template<>
inline void LowLevelAccessor<jfloat>::set(jobject object, jfieldID fieldID, jfloat value) {
    env()->SetFloatField(object, fieldID, value);
}

template<>
inline void LowLevelAccessor<jdouble>::set(jobject object, jfieldID fieldID, jdouble value) {
    env()->SetDoubleField(object, fieldID, value);
}

template<>
inline void LowLevelAccessor<jobject>::set(jobject object, jfieldID fieldID, jobject value) {
    env()->SetObjectField(object, fieldID, value);
}

template<>
inline void LowLevelAccessor<jarray>::set(jobject object, jfieldID fieldID, jarray value) {
    env()->SetObjectField(object, fieldID, static_cast<jobject>(value));
}

template<>
inline void LowLevelAccessor<jobjectArray>::set(jobject object, jfieldID fieldID, jobjectArray value) {
    env()->SetObjectField(object, fieldID, static_cast<jobject>(value));
}

//
// createJavaArray implementations.  Creates primitive arrays
//

template<>
inline jbooleanArray LowLevelAccessor<jboolean>::createJavaArray(int size) {
    return env()->NewBooleanArray(size);
}

template<>
inline jbyteArray LowLevelAccessor<jbyte>::createJavaArray(int size) {
    return env()->NewByteArray(size);
}

template<>
inline jcharArray LowLevelAccessor<jchar>::createJavaArray(int size) {
    return env()->NewCharArray(size);
}

template<>
inline jshortArray LowLevelAccessor<jshort>::createJavaArray(int size) {
    return env()->NewShortArray(size);
}

template<>
inline jintArray LowLevelAccessor<jint>::createJavaArray(int size) {
    return env()->NewIntArray(size);
}

template<>
inline jlongArray LowLevelAccessor<jlong>::createJavaArray(int size) {
    return env()->NewLongArray(size);
}

template<>
inline jfloatArray LowLevelAccessor<jfloat>::createJavaArray(int size) {
    return env()->NewFloatArray(size);
}

template<>
inline jdoubleArray LowLevelAccessor<jdouble>::createJavaArray(int size) {
    return env()->NewDoubleArray(size);
}



//
// Primitive array elements getters
//

template<>
inline jboolean* LowLevelAccessor<jboolean>::getElements(jarray array, jboolean* isCopy) {
    return env()->GetBooleanArrayElements(static_cast<jbooleanArray>(array), isCopy);
}

template<>
inline jbyte* LowLevelAccessor<jbyte>::getElements(jarray array, jboolean* isCopy) {
    return env()->GetByteArrayElements(static_cast<jbyteArray>(array), isCopy);
}

template<>
inline jchar* LowLevelAccessor<jchar>::getElements(jarray array, jboolean* isCopy) {
    return env()->GetCharArrayElements(static_cast<jcharArray>(array), isCopy);
}

template<>
inline jshort* LowLevelAccessor<jshort>::getElements(jarray array, jboolean* isCopy) {
    return env()->GetShortArrayElements(static_cast<jshortArray>(array), isCopy);
}

template<>
inline jint* LowLevelAccessor<jint>::getElements(jarray array, jboolean* isCopy) {
    return env()->GetIntArrayElements(static_cast<jintArray>(array), isCopy);
}

template<>
inline jlong* LowLevelAccessor<jlong>::getElements(jarray array, jboolean* isCopy) {
    return env()->GetLongArrayElements(static_cast<jlongArray>(array), isCopy);
}

template<>
inline jfloat* LowLevelAccessor<jfloat>::getElements(jarray array, jboolean* isCopy) {
    return env()->GetFloatArrayElements(static_cast<jfloatArray>(array), isCopy);
}

template<>
inline jdouble* LowLevelAccessor<jdouble>::getElements(jarray array, jboolean* isCopy) {
    return env()->GetDoubleArrayElements(static_cast<jdoubleArray>(array), isCopy);
}


//
// Primitive array elements releasers
//

template<>
inline void LowLevelAccessor<jboolean>::releaseElements(jarray array, jboolean* elements) {
    env()->ReleaseBooleanArrayElements(static_cast<jbooleanArray>(array), elements, 0);
}

template<>
inline void LowLevelAccessor<jbyte>::releaseElements(jarray array, jbyte* elements) {
    env()->ReleaseByteArrayElements(static_cast<jbyteArray>(array), elements, 0);
}

template<>
inline void LowLevelAccessor<jchar>::releaseElements(jarray array, jchar* elements) {
    env()->ReleaseCharArrayElements(static_cast<jcharArray>(array), elements, 0);
}

template<>
inline void LowLevelAccessor<jshort>::releaseElements(jarray array, jshort* elements) {
    env()->ReleaseShortArrayElements(static_cast<jshortArray>(array), elements, 0);
}

template<>
inline void LowLevelAccessor<jint>::releaseElements(jarray array, jint* elements) {
    env()->ReleaseIntArrayElements(static_cast<jintArray>(array), elements, 0);
}

template<>
inline void LowLevelAccessor<jlong>::releaseElements(jarray array, jlong* elements) {
    env()->ReleaseLongArrayElements(static_cast<jlongArray>(array), elements, 0);
}

template<>
inline void LowLevelAccessor<jfloat>::releaseElements(jarray array, jfloat* elements) {
    env()->ReleaseFloatArrayElements(static_cast<jfloatArray>(array), elements, 0);
}

template<>
inline void LowLevelAccessor<jdouble>::releaseElements(jarray array, jdouble* elements) {
    env()->ReleaseDoubleArrayElements(static_cast<jdoubleArray>(array), elements, 0);
}

//
// Primitive array elements setters
//


template<>
inline void LowLevelAccessor<jboolean>::setElements(jbooleanArray javaArray, jboolean cppArray[], int start, int len) {
    return env()->SetBooleanArrayRegion(javaArray, start, len, cppArray);
}

template<>
inline void LowLevelAccessor<jbyte>::setElements(jbyteArray javaArray, jbyte cppArray[], int start, int len) {
    return env()->SetByteArrayRegion(javaArray, start, len, cppArray);
}

template<>
inline void LowLevelAccessor<jchar>::setElements(jcharArray javaArray, jchar cppArray[], int start, int len) {
    return env()->SetCharArrayRegion(javaArray, start, len, cppArray);
}

template<>
inline void LowLevelAccessor<jshort>::setElements(jshortArray javaArray, jshort cppArray[], int start, int len) {
    return env()->SetShortArrayRegion(javaArray, start, len, cppArray);
}

template<>
inline void LowLevelAccessor<jint>::setElements(jintArray javaArray, jint cppArray[], int start, int len) {
    return env()->SetIntArrayRegion(javaArray, start, len, cppArray);
}

template<>
inline void LowLevelAccessor<jlong>::setElements(jlongArray javaArray, jlong cppArray[], int start, int len) {
    return env()->SetLongArrayRegion(javaArray, start, len, cppArray);
}

template<>
inline void LowLevelAccessor<jfloat>::setElements(jfloatArray javaArray, jfloat cppArray[], int start, int len) {
    return env()->SetFloatArrayRegion(javaArray, start, len, cppArray);
}

template<>
inline void LowLevelAccessor<jdouble>::setElements(jdoubleArray javaArray, jdouble cppArray[], int start, int len) {
    return env()->SetDoubleArrayRegion(javaArray, start, len, cppArray);
}


} // namespace jni_pp
