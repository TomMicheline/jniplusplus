//
// Converters.cpp
// libjni++
//
// Created by Thomas Micheline Apr 12, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#include "jnipp/Converters.hpp"

namespace jni_pp {

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

}