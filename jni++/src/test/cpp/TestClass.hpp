//
// TestClass.hpp
// libjni++
//
// Created by Thomas Micheline Apr 19, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#pragma once

#include <string>
#include <iostream>


struct TestStruct {
    char c;
    int i;
    long l;

    friend std::ostream &operator<<(std::ostream &os, const TestStruct &aStruct) {
        os << "c: " << aStruct.c << " i: " << aStruct.i << " l: " << aStruct.l;
        return os;
    }
};

class TestClass {
public:
    TestClass(const std::string &aString, int anInt, double aDouble) : m_string(aString), m_int(anInt), m_double(aDouble), m_struct({}) {}

    const TestStruct &getStruct() const;
    void setStruct(const TestStruct &aStruct);

    const std::string &getString() const;
    void setString(const std::string &mString);

    int getInt() const;
    void setInt(int mInt);

    double getDouble() const;
    void setDouble(double mDouble);

    friend std::ostream &operator<<(std::ostream &os, const TestClass &aClass);

    void print() {
        std::cout << this << std::endl;
    }

private:
    std::string m_string;
    int m_int;
    double m_double;
    TestStruct m_struct;
};
