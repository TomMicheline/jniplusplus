#
# CMakeLists.txt
# libjni++
#
# Created by Thomas Micheline Mar 26, 2023.
#
# Copyright © 2023 Thomas Micheline All rights reserved.
# Copyright © 2023 VSee Lab, Inc. All rights reserved.
#
# This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
#

cmake_minimum_required(VERSION 3.22)
project(jni++_simple_app)

find_package(Java REQUIRED)
find_package(JNI)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
add_executable(simpleapp SimpleApp.cpp)
target_link_libraries(
        simpleapp
        jni++_static
        ${JAVA_JVM_LIBRARY}
)

install(TARGETS simpleapp)