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
set(CMAKE_CXX_STANDARD 20)

find_package(Java REQUIRED)

set(JAVA_AWT_LIBRARY NotNeeded)
set(JAVA_AWT_INCLUDE_PATH NotNeeded)

if (CMAKE_SYSTEM_NAME MATCHES "Android")
    set(JAVA_INCLUDE_PATH2 NotNeeded)
    set(JAVA_JVM_LIBRARY NotNeeded)
endif ()

find_package(JNI REQUIRED)

if (CMAKE_SYSTEM_NAME MATCHES "Android")
    # make it empty so it doesnt try to link to "NotNeeded"
    set(JAVA_JVM_LIBRARY "")
    set(rpath_flags "")
else ()
    get_filename_component(JAVA_JVM_DIRECTORY ${JAVA_JVM_LIBRARY} DIRECTORY)
    set(rpath_flags "-Wl,-rpath,${JAVA_JVM_DIRECTORY}")
endif ()


set (libheaders
        include/JniPlusPlus.hpp
        include/jnipp/BoxedPrimatives.hpp
        include/jnipp/Converters.hpp
        include/jnipp/Exceptions.hpp
        include/jnipp/InvokersHighLevel.hpp
        include/jnipp/InvokersLowLevel.hpp
        include/jnipp/JniMapping.hpp
        include/jnipp/Loggers.hpp
        include/jnipp/Singletons.hpp
        include/jnipp/SwigSupport.hpp
        include/jnipp/ThreadWrapper.hpp
        include/jnipp/Utilities.hpp
        )

set(libsrc
        src/Converters.cpp
        src/Exceptions.cpp
        src/JvmNativeImpls.cpp
        src/Loggers.cpp
        src/Singletons.cpp
        src/ThreadWrapper.cpp
        src/Utilities.cpp
        )

add_library(objlib OBJECT ${libsrc} ${libheaders})
set_property(TARGET objlib PROPERTY POSITION_INDEPENDENT_CODE 1)
add_library(jni++ SHARED $<TARGET_OBJECTS:objlib>)
add_library(jni++_static STATIC $<TARGET_OBJECTS:objlib>)

target_link_libraries(
        jni++
        ${JAVA_JVM_LIBRARY}
        ${rpath_flags}
)
target_link_libraries(
        jni++_static
        ${JAVA_JVM_LIBRARY}
        ${rpath_flags}
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