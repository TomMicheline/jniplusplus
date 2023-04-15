#
# CMakeLists.txt
# jni++
#
# Created by Thomas Micheline Apr 14, 2023.
#
# Copyright © 2023 Thomas Micheline All rights reserved.
#
# This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
#


cmake_minimum_required(VERSION 3.22)

project(jni++-android-example)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(../../../../../../jni++/src/main/cpp jni++)

add_library(JniPPExample SHARED Example.cpp Example.hpp)

target_link_libraries(JniPPExample jni++)
