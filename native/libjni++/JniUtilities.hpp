//
// JniUtilities.hpp
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
#include <string>
#include <vector>
#include <set>
#include <cassert>

#include "JniLoggers.hpp"

//
// TODO: Clean up this hot mess.  Lots of refactoring required after decoupling from larger project.
//

extern "C"
JNIEXPORT void JNICALL
Java_dev_tmich_jnipp_JavaToNativeExporter_registerSingleton(JNIEnv *env, jclass clazz, jstring jname, jobject obj);

extern "C"
JNIEXPORT void JNICALL
Java_dev_tmich_jnipp_JavaToNativeExporter_unregisterSingleton(JNIEnv *env, jclass clazz, jstring classNameJavaString, jobject obj);

extern "C"
JNIEXPORT void JNICALL
Java_dev_tmich_jnipp_JavaToNativeExporter_unregisterAllSingletons(JNIEnv *env, jclass clazz);

//
// Send logging from JVM to native code.
//
extern "C"
JNIEXPORT void JNICALL
Java_dev_tmich_jnipp_JavaToNativeExporter_nativeLog(JNIEnv *env, jclass clazz, jint level, jstring msg);



//
// This is the native function called from Java that finally calls the main thread function
//

extern "C"
JNIEXPORT jint JNICALL
Java_dev_tmich_jnipp_JavaToNativeExporter_nativeThreadWrapper(JNIEnv *, jclass, jlong);


/***
 *
 * C++ language bindings.
 *
 */


namespace jni_pp {

template<typename T> using set =    std::set<T>;

using ::std::vector;

// For managing startup and shutdown of VM from native code.  Not relevant for managed platforms like Android
bool createVM(jint version, const std::string& classPath);
void destroyVM();

bool isEnvSetup();
JNIEnv *env();
void setEnv(JNIEnv *env);
JavaVM *getVM();
void setVM(JavaVM *vm);

bool attachCurrentThread();
void detachCurrentThread();

void log_vprint(int level, const char *fmt, va_list args);
void log_print(int level, const char *fmt, ...);

void setMinimumLogLevel(LogPriority level);
LogPriority getMinimumLogLevel();

void setLogger(std::shared_ptr<JniLogger> logger);
std::shared_ptr<JniLogger> getLogger();

void setMode(bool debug);
bool isDebugMode();

#define assertm(exp, msg) assert(((void)msg, exp))



typedef struct MethodInfo {
    ~MethodInfo() {
        try {
            env()->DeleteGlobalRef(class_);
        } catch (...) {} // Best effort, ignore errors
    }

    jclass class_;
    jmethodID methodID;
} MethodInfo;

typedef struct FieldInfo {
    ~FieldInfo() {
        try {
            env()->DeleteGlobalRef(class_);
        } catch (...) {} // Best effort, ignore errors
    }

    jclass class_;
    jfieldID fieldID;
} FieldInfo;

jclass getClass(const std::string &name);
MethodInfo *getMethodInfo(const std::string& className, const std::string& methodName, const std::string& signature, bool isStatic, int numParameters);
FieldInfo *getFieldInfo(const std::string& className, const std::string& fieldName, const std::string& signature, bool isStatic);

jstring toJString(const char *s);
jstring toJString(std::string& s);

void setPackageBase(const std::string& packageName);
const std::string& getPackageBase();
void setSwigPackage(const std::string& packageName);
const std::string& getSwigPackage();

jobject getSingletonObject(const std::string& className);

class JniLocalReferenceScope {
public:
	explicit JniLocalReferenceScope(int capacity) {
        if (env()->PushLocalFrame(capacity)) {
            jni_pp::log_print(LOG_ERROR, "PushLocalFrame failed, out of memory.  Punting.");
            return;
        }
        framePushed = true;
    }
	JniLocalReferenceScope() : JniLocalReferenceScope(16) {}

	~JniLocalReferenceScope() {
		(void) releaseLocalRefs(nullptr);
    }

    jobject releaseLocalRefs(jobject savedReference) {
		if (framePushed) {
			framePushed = false;
			return env()->PopLocalFrame(savedReference);
		}
		return savedReference;
	}

private:
    bool framePushed = false;
};


template <typename JniType>
class JniScopedGlobalReference {
public:
	explicit JniScopedGlobalReference(JniType localReference) {
		if (!localReference) {
			// Why try to wrap a null reference?  Probably some other JNI call failed and we didn't notice
			throw std::runtime_error("JniScopedGlobalReference was passed a null local reference");
		}
		globalReference = static_cast<JniType>(env()->NewGlobalRef(localReference));
		if (!globalReference) {
			// Out of memory... what can you do?
			throw std::runtime_error("Out of memory error occurred in NewGlobalRef");
		}

		// Now we can say only moved JniScopedGlobalReferences have null globalReferences
	}

	JniScopedGlobalReference(JniScopedGlobalReference&& other)  noexcept :
			globalReference(other.globalReference) {
		other.globalReference = nullptr;
	}

	virtual ~JniScopedGlobalReference() {
		if (globalReference != nullptr) {
			env()->DeleteGlobalRef(globalReference);
		}
	}

	JniType operator*() const {
		assertm(globalReference, "Dereference of JniScopedGlobalReference after it has been moved is illegal");
		return globalReference;
	}

private:
	JniType globalReference;
};



class no_such_method : public std::runtime_error {
public:
	explicit no_such_method(const std::string &name) : runtime_error(std::string("No such method '") + name + "' has been cached.") {}
};

class no_such_class : public std::runtime_error {
public:
	explicit no_such_class(const std::string &name) : runtime_error(std::string("No such class '") + name + "' has been cached.") {}
};


struct java_exception_details {
	explicit java_exception_details(const jthrowable& jexception)  {
	    exccls = env()->GetObjectClass(jexception);
	    clscls = env()->FindClass("java/lang/Class");
	    frmcls = env()->FindClass("java/lang/StackTraceElement");

	    getName = env()->GetMethodID(clscls, "getName", "()Ljava/lang/String;");
	    getMessage = env()->GetMethodID(exccls, "getMessage", "()Ljava/lang/String;");
	    frameToString = env()->GetMethodID( frmcls, "toString", "()Ljava/lang/String;");
	    getStackTrace = env()->GetMethodID(exccls, "getStackTrace", "()[Ljava/lang/StackTraceElement;");
	    getCause = env()->GetMethodID(exccls, "getCause", "()Ljava/lang/Throwable;");

	    auto jname(jstring(env()->CallObjectMethod(exccls, getName)));
	    char const* utfName(jname == nullptr ? "" : env()->GetStringUTFChars(jname, nullptr));
	    name = utfName;

	    auto jmessage(jstring(env()->CallObjectMethod(jexception, getMessage)));
	    char const* utfMessage(jmessage == nullptr ? "" : env()->GetStringUTFChars(jmessage, nullptr));

	    message = utfMessage;

	    appendStacktrace(jexception);

	    if (jmessage) env()->ReleaseStringUTFChars(jmessage, utfMessage);
	    if (jname) env()->ReleaseStringUTFChars(jname, utfName);
	}

	jclass exccls, clscls, frmcls;
	jmethodID getName, getMessage, frameToString, getStackTrace, getCause;

	std::string name;
	std::string message;
	std::vector<std::string> stacktraceCauses;
	std::vector<std::vector<std::string> > stacktraceFrames;

private:
	void appendStacktrace(const jthrowable& topException) {
		jthrowable jexception(topException);

		// Loop through the causes..
		for (int causeIndex = 0; jexception != nullptr; ++causeIndex) {

			auto frames( (jobjectArray) env()->CallObjectMethod(jexception, getStackTrace) );
			if (!frames) return;

			auto jmessage(jstring(env()->CallObjectMethod(jexception, getMessage)));
			char const* utfMessage(jmessage == nullptr ? "" : env()->GetStringUTFChars(jmessage, nullptr));

			jsize numberOfFrames( env()->GetArrayLength(frames) );
			std::vector<std::string> frameStrings;

			for (jsize frameIndex = 0; frameIndex < numberOfFrames; frameIndex++) {
				jobject frame( env()->GetObjectArrayElement(frames, frameIndex) );

				auto frameAsJString( (jstring) env()->CallObjectMethod(frame, frameToString) );
				const char* frameAsCppString = frameAsJString == nullptr ? "" : env()->GetStringUTFChars(frameAsJString, nullptr);
				frameStrings.emplace_back(frameAsCppString );

				env()->ReleaseStringUTFChars(frameAsJString, frameAsCppString);
				env()->DeleteLocalRef(frameAsJString);
				env()->DeleteLocalRef(frame);
			}

			stacktraceCauses.emplace_back(utfMessage );
			stacktraceFrames.push_back( frameStrings );

			if (jmessage) {
				env()->ReleaseStringUTFChars(jmessage, utfMessage);
				env()->DeleteLocalRef(jmessage);
			}

			jexception = (jthrowable) env()->CallObjectMethod(jexception, getCause);
		}
	}
};

class java_exception : public std::runtime_error {
public:
	explicit java_exception(const java_exception_details& details) :
		runtime_error(std::string("An exception was thrown in JAVA [ ") + details.name + ": " + details.message + " ]") {}
	java_exception(const java_exception& e) = default;
	~java_exception() noexcept override = default;
};

void sendExceptionEvent(jthrowable jt);

inline void checkForExceptions() {
    if (env()->ExceptionCheck()) {
        jthrowable jexception = env()->ExceptionOccurred();
        env()->ExceptionClear();

        java_exception_details details(jexception);

       jni_pp::log_print(LOG_ERROR, "Caught JAVA exception %s: %s", details.name.c_str(), details.message.c_str());
        for (int cause=0; cause<details.stacktraceCauses.size(); ++cause) {
            jni_pp::log_print(LOG_ERROR, "Caused by: %s", details.stacktraceCauses[cause].c_str());
            for (const auto & frame : details.stacktraceFrames[cause]) {
                jni_pp::log_print(LOG_ERROR, "\t%s", frame.c_str());
            }
        }
        jni_pp::log_print(LOG_ERROR, "Throw exception");

        try {
            // Send it to crash reporter if one is configured
            sendExceptionEvent(jexception);
        } catch (std::exception& e) {
			jni_pp::log_print(LOG_ERROR, "Attempt to send Java exception found in native caller threw exception: %s.", e.what());
        } catch (...) {
            jni_pp::log_print(LOG_ERROR, "Attempt to send Java exception found in native caller threw exception.");
        }

        // Going to intentionally leak this since we are shutting down anyway and I'm not 100% sure crashlytics doesn't move this onto another
        // thread.
        // env()->DeleteLocalRef(jexception);

        throw java_exception(details);
    }
}

//
// Pass all info needed to start native thread
//

struct javaThreadArgs {
	int (*start_routine)(void *);
	int priority;
	void *arg;
    const char *name;
};

//
// This is the function passed to pthread as the main thread function.  It attaches the jvm and calls a Java wrapper
// method that, in turn, calls the native wrapper function (above).
//

uint32_t javaThreadWrapper(struct javaThreadArgs *args);


} // namespace jni_pp
