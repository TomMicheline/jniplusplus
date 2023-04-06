//
// JniUtilities.cpp
// libjni++
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
#include <utility>
#include <algorithm>

#include "JniPlusPlus.hpp"
#include "JniUtilities.hpp"

#pragma GCC diagnostic ignored "-Wformat-security"


namespace jni_pp {


static bool debugMode = false;

std::mutex WrapperMapper::mutex;
std::map<std::type_index, const std::string> WrapperMapper::wrapperMap;

void setJavaMinimumLogLevel();

void setMode(bool debug) {
    debugMode = debug;
}

bool isDebugMode() {
    return debugMode;
}

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

std::shared_ptr<JniLogger> gLog = std::make_shared<JniStreamLogger>();
LogPriority gMinimumLogLevel = LOG_DEBUG;

void setJavaMinimumLogLevel() {
    if (isEnvSetup()) {
        static StaticMethod<void, int> jSetMinimumLogLevel("dev.tmich.jnipp.JavaToNativeExporter", "setMinimumLogLevel");
        jSetMinimumLogLevel(gMinimumLogLevel);
    }
}

void setMinimumLogLevel(LogPriority level) {
    gMinimumLogLevel = level;
    setJavaMinimumLogLevel();
}

LogPriority getMinimumLogLevel() {
    return gMinimumLogLevel;
}

void setLogger(std::shared_ptr<JniLogger> logger)
{
    gLog = std::move(logger);
}

std::shared_ptr<JniLogger> getLogger()
{
    return gLog;
}

void log_vprint(int level, const char *fmt, va_list args) {
	char dest[1024];
	vsnprintf(dest, 1024, fmt, args);
    switch (level) {
        default:
        case LOG_DEBUG:
            gLog->debug(dest);
            break;
        case LOG_WARN:
            gLog->warning(dest);
            break;
        case LOG_ERROR:
            gLog->error(dest);
            break;
    }
}

void log_print(int level, const char *fmt, ...) {
	va_list argptr;
	va_start(argptr, fmt);
    log_vprint(level, fmt, argptr);
	va_end(argptr);
}

void sendExceptionEvent(jthrowable jt) {
    // TODO: make pluggable crash reporter system so it can be sent to places like crashlytics
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

#pragma clang diagnostic push

template <typename CppType>
CppType* getSwigPointer(jobject obj) {
    static InstanceField<long> pointerField(SwigWrapper<CppType>::getClassName());
    auto pointerAsLong = pointerField.get(obj);
    return *((CppType**)&pointerAsLong);
//    return reinterpret_cast<CppType*>(pointerAsLong);
}


jvalue convertToJValue(std::string& arg) {
	jvalue jv;
	jv.l = env()->NewStringUTF(arg.c_str());
	return jv;
}

jvalue convertToJValue(const char * arg) {
    jvalue jv;
    if (!arg) jv.l = nullptr;
    else jv.l = env()->NewStringUTF(arg);
	return jv;
}

jvalue convertToJValue(bool arg) {
	jvalue jv;
	jv.z = arg;
	return jv;
}

// This is a potential problem.  jbyte is signed so it isn't clear what type should be used.  Maybe short?
jvalue convertToJValue(unsigned char arg) {
	jvalue jv;
	jv.b = arg;
	return jv;
}

jvalue convertToJValue(char arg) {
	jvalue jv;
	jv.c = (jchar) (unsigned char) arg;
	return jv;
}

jvalue convertToJValue(short arg) {
	jvalue jv;
	jv.s = arg;
	return jv;
}

jvalue convertToJValue(int arg) {
	jvalue jv;
	jv.i = arg;
	return jv;
}

jvalue convertToJValue(long arg) {
    jvalue jv;
    jv.j = arg;
    return jv;
}

jvalue convertToJValue(long long arg) {
    jvalue jv;
    jv.j = arg;
    return jv;
}

jvalue convertToJValue(float arg) {
	jvalue jv;
	jv.f = arg;
	return jv;
}

jvalue convertToJValue(double arg) {
	jvalue jv;
	jv.d = arg;
	return jv;
}
    
jvalue convertToJValue(jobject arg) {
	jvalue jv;
	jv.l = arg;
	return jv;
}

jstring toJString(const char *s) {
	return jstring(convertToJValue(s).l);
}

jstring toJString(std::string& s) {
	return jstring(convertToJValue(s).l);
}
#pragma clang diagnostic pop

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

std::mutex singleton_mutex;
std::map<std::string, jobject> singleton_map;

jobject getSingletonObject(const std::string& className) {
    std::unique_lock<std::mutex> lock(singleton_mutex);
    auto it = singleton_map.find(className);
    if (it == singleton_map.end()) {
        return nullptr;
    }
    return it->second;
}

void clearSingletonObjects() {
    std::unique_lock<std::mutex> lock(singleton_mutex);
    for (auto& pair : singleton_map) {
        env()->DeleteGlobalRef(pair.second);
    }
    singleton_map.clear();
}

void registerSingleton(const std::string& className, jobject obj) {
    std::unique_lock<std::mutex> lock(singleton_mutex);
    auto it = singleton_map.find(className);
    // Only remove the singleton if both the name and object match
    if (it != singleton_map.end()) {
        if (env()->IsSameObject(it->second, obj)) {
            // already mapped
            return;
        } else {
            // Free old reference
            env()->DeleteGlobalRef(it->second);
        }
    }
    singleton_map[className] = env()->NewGlobalRef(obj);
}

void unregisterSingleton(const std::string& className, jobject obj) {
    std::unique_lock<std::mutex> lock(singleton_mutex);
    auto it = singleton_map.find(className);
    // Only remove the singleton if both the name and object match
    if (it != singleton_map.end() && env()->IsSameObject(it->second, obj)) {
        env()->DeleteGlobalRef(it->second);
        singleton_map.erase(it);
    }
}

uint32_t javaThreadWrapper(struct javaThreadArgs *args)
{
	static StaticMethod<int, const char *, long> jBindToAppClassLoader("JavaToNativeExporter", "bindToAppClassLoader", "(Ljava/lang/String;J)I");

    bool doDetachThread = jni_pp::attachCurrentThread();

    //
    // Call to Java which will, in turn, call ..._nativeThreadWrapper (above).  With the Java frames from classes in our
    // application on the call stack, JNI functions to lookup classes will use the correct class loader.  *sigh*  Super painful.
    //
    uint32_t callbackResult = 0;
    const char* name = args->name;  // save name since args is freed during the bind operation...

    bool successful = false;
    std::string msg;
    try {
    	callbackResult = (uint32_t) jBindToAppClassLoader(args->name, (long)args);
    	successful = true;
	} catch (const std::exception& ex) {
		msg = ex.what();
    } catch (const std::string& ex) {
    	msg = ex;
    } catch (...) {
    	msg = "Unknown exception";
	}

    if (!successful) {
        jni_pp::log_print(LOG_DEBUG, "bindToAppClassLoader for thread %s threw exception!  Error: %s", name, msg.c_str());
        doDetachThread = false;
    }

    if (doDetachThread) {
        jni_pp::detachCurrentThread();
    }

    return callbackResult;
}

}  // Namespace jni_pp


using namespace jni_pp;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "UnusedParameter"

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
	log_print(LOG_DEBUG, "Native: JNI_OnLoad called.  Caching VM.");
	jni_pp::setVM(vm);
    initializeEnvironment();
    return JNI_VERSION_1_6;
}

extern "C"
void Java_dev_tmich_jnipp_JavaToNativeExporter_registerSingleton(JNIEnv *env, jclass clazz, jstring jname, jobject obj) {
    std::string className = ToCppConverter<std::string>::convertToCpp((jobject)jname);
    registerSingleton(className, obj);
}

extern "C"
void Java_dev_tmich_jnipp_JavaToNativeExporter_unregisterSingleton(JNIEnv *env, jclass clazz, jstring classNameJavaString, jobject obj) {
    std::string className = ToCppConverter<std::string>::convertToCpp((jobject)classNameJavaString);
    unregisterSingleton(className, obj);
}

extern "C"
void Java_dev_tmich_jnipp_JavaToNativeExporter_unregisterAllSingletons(JNIEnv *env, jclass clazz) {
    clearSingletonObjects();
}

extern "C"
void Java_dev_tmich_jnipp_JavaToNativeExporter_nativeLog(JNIEnv *env, jclass clazz, jint level, jstring jmsg) {
    std::string msg = ToCppConverter<std::string>::convertToCpp((jobject)jmsg);
    jni_pp::log_print(level, msg.c_str());
}

extern "C"
jint Java_dev_tmich_jnipp_JavaToNativeExporter_nativeThreadWrapper(JNIEnv *env, jclass cls, jlong ptrToJavaThreadArgs)
{
    auto *args = (struct javaThreadArgs *) ptrToJavaThreadArgs;
	auto start_routine = args->start_routine;
	void *arg = args->arg;
	const char* name = args->name;

    //
    // Better call this here, after we have attached to correct class loader than in javaThreadWrapper
    //
//    if ( args->priority != thread_PriorityNormal) {
//        // TODO: Set priority
//    }

    free(args);

	jint result = -1;

    bool caughtException = true;
	try {
		result = start_routine(arg);
        caughtException = false;
	} catch (const std::exception& ex) {
		jni_pp::log_print(LOG_DEBUG, "nativeThreadWrapper caught exception for thread %s. Returning -1!  Error: %s", name, ex.what());
    } catch (const std::string& ex) {
		jni_pp::log_print(LOG_DEBUG, "nativeThreadWrapper caught exception for thread %s. Returning -1!  Error: %s", name, ex.c_str());
    } catch (...) {
		jni_pp::log_print(LOG_DEBUG, "nativeThreadWrapper caught exception for thread %s. Returning -1!  Error: Unknown Exception", name);
	}

    assert((!caughtException || !jni_pp::isDebugMode()) && "Thread threw uncaught exception in debug mode");

    return result;
}
#pragma clang diagnostic pop
