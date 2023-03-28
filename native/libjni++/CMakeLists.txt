#
# CMakeLists.txt
# libjni++
#
# Created by Thomas Micheline Mar 26, 2023.
#
# Copyright (c) 2023, Thomas Micheline
# Copyright (c) 2023, VSee Lab, Inc.
#
# This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
#


cmake_minimum_required(VERSION 3.24)
project(libjni++)

find_package(Java REQUIRED)

message("JAVA_HOME = ${JAVA_HOME}")
message("Java_JAVA_EXECUTABLE = ${Java_JAVA_EXECUTABLE}")
message("Java_JAVAC_EXECUTABLE = ${Java_JAVAC_EXECUTABLE}")
message("Java_JAVAH_EXECUTABLE = ${Java_JAVAH_EXECUTABLE}")
message("Java_JAVADOC_EXECUTABLE = ${Java_JAVADOC_EXECUTABLE}")
message("Java_VERSION_STRING = ${Java_VERSION_STRING}")
message("Java_VERSION = ${Java_VERSION}")

find_package(JNI)

if (JNI_FOUND)
    # -- JNI_INCLUDE_DIRS=C:/Program Files/Java/jdk1.8.0_191/include;C:/Program Files/Java/jdk1.8.0_191/include/win32;C:/Program Files/Java/jdk1.8.0_191/include
    # -- JNI_LIBRARIES=C:/Program Files/Java/jdk1.8.0_191/lib/jawt.lib;C:/Program Files/Java/jdk1.8.0_191/lib/jvm.lib
    message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
    message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
endif()

include_directories(.)
include_directories(${JNI_INCLUDE_DIRS})

add_library(jni++
        SHARED
        Converters.hpp
        HighLevelInvokers.hpp
        JniPlusPlus.hpp
        JniUtilities.cpp
        JniUtilities.hpp
        LowLevelInvokers.hpp
        JniLoggers.cpp
        JniLoggers.hpp)

target_link_libraries(jni++ ${JNI_LIBRARIES})
