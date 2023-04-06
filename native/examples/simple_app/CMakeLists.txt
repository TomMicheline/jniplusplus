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

get_filename_component(JAVA_JVM_DIRECTORY ${JAVA_JVM_LIBRARY} DIRECTORY)

include_directories("${lib_src_dir}")
include_directories(${JNI_INCLUDE_DIRS})

add_executable(simpleapp SimpleApp.cpp)

target_link_libraries(
        simpleapp
        jni++_static
        ${JAVA_JVM_LIBRARY}
#        "-Wl,-rpath,${JAVA_JVM_DIRECTORY}"
)

set(CMAKE_INSTALL_RPATH ${JAVA_JVM_DIRECTORY})

install(TARGETS simpleapp)