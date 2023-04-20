//
// TestClass.cpp
// libjni++
//
// Created by Thomas Micheline Apr 19, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#include "TestClass.hpp"

const std::string &TestClass::getString() const {
    return m_string;
}

void TestClass::setString(const std::string &mString) {
    m_string = mString;
}

int TestClass::getInt() const {
    return m_int;
}

void TestClass::setInt(int mInt) {
    m_int = mInt;
}

double TestClass::getDouble() const {
    return m_double;
}

void TestClass::setDouble(double mDouble) {
    m_double = mDouble;
}

const TestStruct &TestClass::getStruct() const {
    return m_struct;
}

void TestClass::setStruct(const TestStruct &mStruct) {
    m_struct = mStruct;
}

std::ostream &operator<<(std::ostream &os, const TestClass &aClass) {
    os << "m_string: " << aClass.m_string << " m_int: " << aClass.m_int << " m_double: " << aClass.m_double
       << " m_struct: " << aClass.m_struct;
    return os;
}
