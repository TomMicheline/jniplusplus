//
// JniUtilities.cpp
// jni++
//
// Created by Thomas Micheline Jun 22, 2013.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
// Copyright © 2023 VSee Lab, Inc. All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#include <climits>
#include <map>
#include <algorithm>

#include "JniPlusPlus.hpp"
#include "jnipp/Utilities.hpp"
#include "jnipp/References.hpp"

#pragma GCC diagnostic ignored "-Wformat-security"


namespace jni_pp {


std::mutex WrapperMapper::mutex;
std::map<std::type_index, const std::string> WrapperMapper::wrapperMap;

void setJavaMinimumLogLevel();

std::recursive_mutex javaEnv_mutex;
thread_local JNIEnv* envInstancePtr(nullptr);

StaticMethod<bool, jobject> jIsExportable("dev.tmich.jnipp.JavaToNativeExporter",
                                                "isExportable",
                                                "(Ljava/lang/reflect/Member;)Z");
StaticMethod<jobject, std::string, std::string, bool, int> jLookupJavaMember("dev.tmich.jnipp.JavaToNativeExporter",
                                                                             "lookupJavaMember",
                                                                             "(Ljava/lang/String;Ljava/lang/String;ZI)Ljava/lang/reflect/AccessibleObject;");
StaticMethod<jobject, std::string, std::string, bool> jLookupJavaField("dev.tmich.jnipp.JavaToNativeExporter",
                                                                       "lookupJavaField",
                                                                       "(Ljava/lang/String;Ljava/lang/String;Z)Ljava/lang/reflect/Field;");
InstanceMethod<jobject> jGetFieldClass("java/lang/reflect/Field", "getDeclaringClass", "()Ljava/lang/Class;");


void setEnv(JNIEnv* env) {
	envInstancePtr = env;
}

JNIEnv *env() {
    std::lock_guard<std::recursive_mutex> lk(javaEnv_mutex);

	if (!envInstancePtr) {
		log_print(LOG_DEBUG, "Native: envInstancePtr not yet set");
		JavaVM *vm = getVM();
		if (!vm) {
			log_print(LOG_ERROR, "vm not set... throwing exception");
			throw std::runtime_error("Cannot get JNI environment when Java VM has not been cached");
		}

		JNIEnv *env;
		int result;
		if ((result = vm->GetEnv((void **)&env, JNI_VERSION_1_2)) != JNI_OK) {
			switch (result) {
			case JNI_EDETACHED:
				log_print(LOG_ERROR, "thread detached");
				throw std::runtime_error("Cannot get JNI environment from detached thread");
			case JNI_EVERSION:
				log_print(LOG_ERROR, "version problem");
				throw std::runtime_error("JNI version 1.2 not supported");
			default:
				log_print(LOG_ERROR, "unknown error: %d", result);
				throw std::runtime_error("Unknown error getting JNI environment");
			}
		}

		log_print(LOG_DEBUG, "Native: got env, setting it");
		setEnv(env);
	}

	return envInstancePtr;
}

bool isEnvSetup() {
    return (envInstancePtr != nullptr);
}
    
//
// All threads trying to access the Java VM will block until it becomes available.
//
JavaVM* GlobalVM;
std::condition_variable javaVM_cv;
std::mutex javaVM_mutex;

bool createVM(jint version, const std::string& classPath)
{
    {
        std::lock_guard<std::mutex>  lock(javaVM_mutex);

        JavaVM *_vm;
        JNIEnv * _env;

        JavaVMInitArgs vm_args; /* JDK/JRE 6 VM initialization arguments */
        auto *options = new JavaVMOption[1];
        options[0].optionString = strdup(("-Djava.class.path=" + classPath).c_str());
        // std::cerr << "class path cmd line option = '" << options[0].optionString << "'" << std::endl;
        vm_args.version = version;
        vm_args.nOptions = 1;
        vm_args.options = options;
        vm_args.ignoreUnrecognized = false;

        /* load and initialize a Java VM, return a JNI interface pointer in env */
        auto result = JNI_CreateJavaVM(&_vm, (void **) &_env, &vm_args);

        // Cleanup
        delete options[0].optionString;
        delete[] options;

        if (result != JNI_OK) {
            return false;
        }

        GlobalVM = _vm;
        envInstancePtr = _env;
    }

    // Make sure we have the required support class in the classpath.
    if (!envInstancePtr->FindClass("dev/tmich/jnipp/JavaToNativeExporter")) {
        getLogger()->error("jni++ support code not in classpath: " + classPath);
        return false;
    }

    initializeEnvironment();
    javaVM_cv.notify_all();

    return true;
}

void destroyVM()
{
    std::lock_guard<std::mutex>  lock(javaVM_mutex);
    if (GlobalVM != nullptr) {
        GlobalVM->DestroyJavaVM();
        GlobalVM = nullptr;
        envInstancePtr = nullptr;  // What about other threads?  Do we need to keep track of them?
    }
}

void setVM(JavaVM *vm) {
    {
        std::lock_guard<std::mutex>  lock(javaVM_mutex);
    	GlobalVM = vm;
    }
    javaVM_cv.notify_all();
}

JavaVM *getVM() {
    std::unique_lock<std::mutex> lock(javaVM_mutex);
    while(GlobalVM == nullptr)
    {
    	javaVM_cv.wait(lock);
    }
	return GlobalVM;
}


bool attachCurrentThread() {
  if (isEnvSetup()) return true;

  JavaVM* vm = getVM();
  if (!vm) {
    return false;
  }

  JNIEnv *env;
  if (vm->GetEnv((void **)&env, JNI_VERSION_1_2) == JNI_OK) {
    return false;
  }

  if (vm->AttachCurrentThread(reinterpret_cast<void **>(&env), nullptr) != 0) {
    return false;
  }

  setEnv(env);
  return true;
}

void detachCurrentThread() {
  JavaVM* vm = getVM();
  if (vm) {
    vm->DetachCurrentThread();
  }
}

void initializeEnvironment() {
    //
    // These methods are, depending on the situation, called from getMethodInfo so they need to have their method info cached
    // before any other code attempts to call getMethodInfo because otherwise it would be called recursively and deadlock.  Because
    // they are configured with the full signature and are in JavaToNativeExporter, they avoid calling either method before
    // they are cached.  Also, this causes JavaToNativeExporter class to be cached which has to happen before any new threads are
    // created by the native code since they need to be able to call a method on that class before their ClassLoader is configured
    // correctly.
    //
    jIsExportable.getMethodInfo();
    jLookupJavaMember.getMethodInfo();
    jLookupJavaField.getMethodInfo();
    jGetFieldClass.getMethodInfo();

    //
    // Pass the current minimum log level to Java.  It might have been set before we created the VM or were attached.
    //
    setJavaMinimumLogLevel();
}

std::map<std::string, FieldInfo*> javaFieldCache;
std::map<std::string, MethodInfo*> javaMethodCache;
std::map<std::string, jclass> javaClassCache;
std::recursive_mutex cacheMutex;

//
// Check the cache to find the class object.  If it isn't there use JNI to find it and store it in
// the cache.
//
jclass getClass(const std::string &name) {
    jclass cls = javaClassCache[name];
    if (cls) {
        return cls;
    }

    JniLocalReferenceScope  refs; // Clean up any local refs created in this method

    std::string classDescriptor;
    std::replace_copy(name.begin(), name.end(), std::back_inserter<std::string>(classDescriptor), '.', '/');
    cls = env()->FindClass(classDescriptor.c_str());
    assertm(cls != nullptr, "Class lookup failed");

    auto globalJClass = jclass(env()->NewGlobalRef(cls));

    javaClassCache[name] = globalJClass;

    return globalJClass;
}


//
// get the MethodInfo for this Method (InstanceMethod, StaticMethod, or Constructor)
//
MethodInfo  *getMethodInfo(const std::string& className, const std::string& methodName, const std::string& signature, bool isStatic, int numParameters) {
    JniLocalReferenceScope refs;  // Cleans up any local references created in this method

    std::lock_guard<std::recursive_mutex> lk(cacheMutex);
    std::string key = className + "." + methodName;
    if (!signature.empty()) {
        key += ":" + signature;
    } else {
        key += ":" + std::to_string(numParameters);
    }
    log_print(LOG_DEBUG, "Looking up method '%s'.", key.c_str());
    auto mi = javaMethodCache[key];

    if (!mi) {
        //
        // First time we have looked up this exact method.  Either look it up using JNI if we have a signature or
        // using Java if we don't.
        //
        mi = new MethodInfo();

        //
        // First find the class.
        //
        mi->class_ = getClass(className);
        assertm(mi->class_, "Should never happen due to assert in getClass");

        //
        // Now get the method.  Different mechanism depending on whether we have a signature or not.
        //
        if (!signature.empty()) {
            //
            // We have a signature so use JNI to lookup the exact method.
            //
            if (isStatic) {
                mi->methodID = env()->GetStaticMethodID(mi->class_, methodName.c_str(), signature.c_str());
            } else {
                mi->methodID = env()->GetMethodID(mi->class_, methodName.c_str(), signature.c_str());
            }
            assertm(mi->methodID, "methodID lookup failure.  Check signature");

            // We have the method but check to see if it is "exportable."  If a class is in an export required package
            // it must be annotated with @ExportToNative to be "exportable" UNLESS it is is JavaToNativeExporter which
            // is always exportable (to prevent an infinite loop/deadlock right here...)
            if (className != "dev.tmich.jnipp.JavaToNativeExporter") {
                jobject methodObject = env()->ToReflectedMethod(mi->class_, mi->methodID, jboolean(isStatic));
                assertm(methodObject, "ToReflectedMethod should not fail");
                assertm(jIsExportable(methodObject), "Method is not exported!  Annotate with ExportToNative!");
            }
        } else {
            //
            // Have java scan through all the methods looking for the one we want.  All exportability tests are baked in.
            // Returns a Method jobject.  Use JNI to convert to methodID.
            //
            jobject methodObject = jLookupJavaMember(className, methodName, isStatic, numParameters);
            assertm(methodObject, "Method  couldn't be exported");

            mi->methodID = env()->FromReflectedMethod(methodObject);
            assertm(mi->methodID, "FromReflectedMethod shouldn't fail.");
        }

        javaMethodCache[key] = mi;
    }
    return mi;
}




//
// get the FieldInfo for this Field (InstanceField, StaticField, or Constructor)
//
FieldInfo  *getFieldInfo(const std::string& className, const std::string& fieldName, const std::string& signature, bool isStatic) {

    JniLocalReferenceScope refs;  // Cleans up any local references created in this field

    std::lock_guard<std::recursive_mutex> lk(cacheMutex);
    std::string key = className + "." + fieldName;
    log_print(LOG_DEBUG, "Looking up field '%s'.", key.c_str());
    auto fi = javaFieldCache[key];

    if (!fi) {
        jobject fieldObject;

        //
        // First time we have looked up this exact field.  Either look it up using JNI if we have a signature or
        // using Java if we don't.
        //
        fi = new FieldInfo();

        //
        // First find the class.
        //
        jclass class_ = getClass(className);
        assertm(class_, "Should never happen due to assert in getClass");

        //
        // Now get the field.  Different mechanism depending on whether we have a signature or not.
        //
        if (!signature.empty()) {
            //
            // We have a signature so use JNI to lookup the exact field.
            //
            if (isStatic) {
                fi->fieldID = env()->GetStaticFieldID(class_, fieldName.c_str(), signature.c_str());
            } else {
                fi->fieldID = env()->GetFieldID(class_, fieldName.c_str(), signature.c_str());
            }
            assertm(fi->fieldID, "fieldID lookup failure.  Check signature.");

            //
            // Next convert fieldID to java.lang.reflect.Field object
            //
            fieldObject = env()->ToReflectedField(class_, fi->fieldID, jboolean(isStatic));
            assertm(fieldObject, "ToReflectedField should not fail");

            // We have the field but check to see if it is "exportable."
            assertm(jIsExportable(fieldObject), "Field is not exported!  Annotate with ExportToNative!");
        } else {
            //
            // Have java scan through all the fields looking for the one we want.  All exportability tests are baked in.
            // Returns a Field jobject.  Use JNI to convert to fieldID.
            //
            fieldObject = jLookupJavaField(className, fieldName, isStatic);
            assertm(fieldObject, "Field  couldn't be exported");

            fi->fieldID = env()->FromReflectedField(fieldObject);
            assertm(fi->fieldID, "FromReflectedField shouldn't fail.");
        }

        //
        // If a static field is in a superclass, accessing it through the base class jclass seems to cause an exception.  Now that we have the fieldObject,
        // ask IT what it's declaring class should be.
        //
        // Maybe only need this for static fields?  In base classes?  We know if it is static but don't really know the declaring class here.
        //
        auto localref = (jclass) jGetFieldClass(fieldObject);
        fi->class_ = (jclass) env()->NewGlobalRef(localref);
        env()->DeleteLocalRef(localref);
        assertm(fi->class_, "Field.getDeclaringClass shouldn't return null");

        javaFieldCache[key] = fi;
    }
    return fi;
}

template <typename CppType>
CppType* getSwigPointer(jobject obj) {
    static InstanceField<long> pointerField(SwigWrapper<CppType>::getClassName());
    auto pointerAsLong = pointerField.get(obj);
    return *((CppType**)&pointerAsLong);
//    return reinterpret_cast<CppType*>(pointerAsLong);
}

static std::string javaBasePackageName;
static std::string swigPackageName;

void setPackageBase(const std::string& packageName) {
    javaBasePackageName = packageName;
}

const std::string& getPackageBase() {
    return javaBasePackageName;
}

void setSwigPackage(const std::string& packageName) {
    swigPackageName = packageName;
}

const std::string& getSwigPackage() {
    return swigPackageName;
}

}  // Namespace jni_pp
