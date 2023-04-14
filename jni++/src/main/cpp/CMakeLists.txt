#
# CMakeLists.txt
# jni++
#
# Created by Thomas Micheline Mar 26, 2023.
#
# Copyright (c) 2023, Thomas Micheline
# Copyright (c) 2023, VSee Lab, Inc.
#
# This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
#


cmake_minimum_required(VERSION 3.22)
project(libjni++)

set(CMAKE_INSTALL_PREFIX "${install_dir}" CACHE PATH "..." FORCE)
set(CMAKE_CXX_STANDARD 17)

find_package(Java REQUIRED)

#message("JAVA_HOME = ${JAVA_HOME}")
#message("Java_JAVA_EXECUTABLE = ${Java_JAVA_EXECUTABLE}")
#message("Java_JAVAC_EXECUTABLE = ${Java_JAVAC_EXECUTABLE}")
#message("Java_JAVAH_EXECUTABLE = ${Java_JAVAH_EXECUTABLE}")
#message("Java_JAVADOC_EXECUTABLE = ${Java_JAVADOC_EXECUTABLE}")
#message("Java_VERSION_STRING = ${Java_VERSION_STRING}")
#message("Java_VERSION = ${Java_VERSION}")

find_package(JNI)

#if (JNI_FOUND)
#    # -- JNI_INCLUDE_DIRS=C:/Program Files/Java/jdk1.8.0_191/include;C:/Program Files/Java/jdk1.8.0_191/include/win32;C:/Program Files/Java/jdk1.8.0_191/include
#    # -- JNI_LIBRARIES=C:/Program Files/Java/jdk1.8.0_191/lib/jawt.lib;C:/Program Files/Java/jdk1.8.0_191/lib/jvm.lib
#    message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
#    message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
#endif()

get_filename_component(JAVA_JVM_DIRECTORY ${JAVA_JVM_LIBRARY} DIRECTORY)

set (libheaders
        include/JniPlusPlus.hpp
        include/jnipp/Converters.hpp
        include/jnipp/InvokersHighLevel.hpp
        include/jnipp/Utilities.hpp
        include/jnipp/InvokersLowLevel.hpp
        include/jnipp/Loggers.hpp
        include/jnipp/Exceptions.hpp
        include/jnipp/JvmNativeImpls.hpp
        include/jnipp/Singletons.hpp
        include/jnipp/ThreadWrapper.hpp
        )

set(libsrc
        src/Utilities.cpp
        src/Loggers.cpp
        src/Exceptions.cpp
        src/JvmNativeImpls.cpp
        src/Singletons.cpp
        src/ThreadWrapper.cpp
        src/Converters.cpp)

add_library(objlib OBJECT ${libsrc} ${libheaders})
set_property(TARGET objlib PROPERTY POSITION_INDEPENDENT_CODE 1)
add_library(jni++ SHARED $<TARGET_OBJECTS:objlib>)
add_library(jni++_static STATIC $<TARGET_OBJECTS:objlib>)

target_link_libraries(
        jni++
        ${JAVA_JVM_LIBRARY}
        "-Wl,-rpath,${JAVA_JVM_DIRECTORY}"
)
target_link_libraries(
        jni++_static
        ${JAVA_JVM_LIBRARY}
        "-Wl,-rpath,${JAVA_JVM_DIRECTORY}"
)

set_target_properties(jni++ PROPERTIES PUBLIC_HEADER "${libheaders}")

# Include directories for local build of objlib
target_include_directories(objlib PUBLIC "include")
target_include_directories(objlib PUBLIC ${JNI_INCLUDE_DIRS})

# Export header dirs for dependant builds
target_include_directories(jni++ PUBLIC "include")
target_include_directories(jni++_static PUBLIC "include")
target_include_directories(jni++ PUBLIC ${JNI_INCLUDE_DIRS})
target_include_directories(jni++_static PUBLIC ${JNI_INCLUDE_DIRS})

install(TARGETS jni++ jni++_static
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include
        )
install(DIRECTORY "include"
        DESTINATION "${install_dir}"
        )