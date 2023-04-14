//
// JniPPExceptions.cpp
// jni++
//
// Created by Thomas Micheline Apr 12, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#include "jnipp/Exceptions.hpp"
#include "jnipp/Utilities.hpp"

namespace jni_pp {

void checkForExceptions() {
    if (env()->ExceptionCheck()) {
        jthrowable jexception = env()->ExceptionOccurred();
        env()->ExceptionClear();

        java_exception_details details(jexception);

        jni_pp::log_print(LOG_ERROR, "Caught JAVA exception %s: %s", details.name.c_str(), details.message.c_str());
        for (std::vector<std::string>::size_type cause = 0; cause < details.stacktraceCauses.size(); ++cause) {
            jni_pp::log_print(LOG_ERROR, "Caused by: %s", details.stacktraceCauses[cause].c_str());
            for (const auto &frame: details.stacktraceFrames[cause]) {
                jni_pp::log_print(LOG_ERROR, "\t%s", frame.c_str());
            }
        }
        jni_pp::log_print(LOG_ERROR, "Throw exception");

        try {
            // Send it to crash reporter if one is configured
            sendExceptionEvent(jexception);
        } catch (std::exception &e) {
            jni_pp::log_print(LOG_ERROR, "Attempt to send Java exception found in native caller threw exception: %s.",
                              e.what());
        } catch (...) {
            jni_pp::log_print(LOG_ERROR, "Attempt to send Java exception found in native caller threw exception.");
        }

        // Going to intentionally leak this since we are shutting down anyway and I'm not 100% sure crashlytics doesn't move this onto another
        // thread.
        // env()->DeleteLocalRef(jexception);

        throw java_exception(details);
    }
}

void sendExceptionEvent(jthrowable jt) {
    // TODO: make pluggable crash reporter system so it can be sent to places like crashlytics
}

java_exception_details::java_exception_details(jthrowable const &jexception) {
    exccls = env()->GetObjectClass(jexception);
    clscls = env()->FindClass("java/lang/Class");
    frmcls = env()->FindClass("java/lang/StackTraceElement");

    getName = env()->GetMethodID(clscls, "getName", "()Ljava/lang/String;");
    getMessage = env()->GetMethodID(exccls, "getMessage", "()Ljava/lang/String;");
    frameToString = env()->GetMethodID(frmcls, "toString", "()Ljava/lang/String;");
    getStackTrace = env()->GetMethodID(exccls, "getStackTrace", "()[Ljava/lang/StackTraceElement;");
    getCause = env()->GetMethodID(exccls, "getCause", "()Ljava/lang/Throwable;");

    auto jname(jstring(env()->CallObjectMethod(exccls, getName)));
    char const *utfName(jname == nullptr ? "" : env()->GetStringUTFChars(jname, nullptr));
    name = utfName;

    auto jmessage(jstring(env()->CallObjectMethod(jexception, getMessage)));
    char const *utfMessage(jmessage == nullptr ? "" : env()->GetStringUTFChars(jmessage, nullptr));

    message = utfMessage;

    appendStacktrace(jexception);

    if (jmessage) env()->ReleaseStringUTFChars(jmessage, utfMessage);
    if (jname) env()->ReleaseStringUTFChars(jname, utfName);
}

void java_exception_details::appendStacktrace(jthrowable const &topException) {
    jthrowable jexception(topException);

    // Loop through the causes...
    while (jexception != nullptr) {

        auto frames((jobjectArray) env()->CallObjectMethod(jexception, getStackTrace));
        if (!frames) return;

        auto jmessage(jstring(env()->CallObjectMethod(jexception, getMessage)));
        char const *utfMessage(jmessage == nullptr ? "" : env()->GetStringUTFChars(jmessage, nullptr));

        jsize numberOfFrames(env()->GetArrayLength(frames));
        std::vector<std::string> frameStrings;

        for (jsize frameIndex = 0; frameIndex < numberOfFrames; frameIndex++) {
            jobject frame(env()->GetObjectArrayElement(frames, frameIndex));

            auto frameAsJString((jstring) env()->CallObjectMethod(frame, frameToString));
            const char *frameAsCppString =
                    frameAsJString == nullptr ? "" : env()->GetStringUTFChars(frameAsJString, nullptr);
            frameStrings.emplace_back(frameAsCppString);

            env()->ReleaseStringUTFChars(frameAsJString, frameAsCppString);
            env()->DeleteLocalRef(frameAsJString);
            env()->DeleteLocalRef(frame);
        }

        stacktraceCauses.emplace_back(utfMessage);
        stacktraceFrames.push_back(frameStrings);

        if (jmessage) {
            env()->ReleaseStringUTFChars(jmessage, utfMessage);
            env()->DeleteLocalRef(jmessage);
        }

        jexception = (jthrowable) env()->CallObjectMethod(jexception, getCause);
    }
}

}