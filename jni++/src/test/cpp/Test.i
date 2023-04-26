//
// Test.i
// jni++
//
// Created by Thomas Micheline Jun 22, 2013.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
// Copyright © 2023 VSee Lab, Inc. All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

%module Test

%{
#include "TestClass.hpp"
%}

%ignore operator<<;

%include "std_string.i"
%include "TestClass.hpp"
