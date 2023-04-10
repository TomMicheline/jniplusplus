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

project(jni++)

set(CMAKE_CXX_STANDARD 17)

set(install_dir "${CMAKE_CURRENT_LIST_DIR}/../install")
set(lib_src_dir "${CMAKE_CURRENT_LIST_DIR}/../libjni++/src/main/cpp")

add_subdirectory(../libjni++/src/main/cpp libjni++)
add_subdirectory(../examples/simple_app examples)
