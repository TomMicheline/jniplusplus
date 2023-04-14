project(jni++_tests)

find_package(Java REQUIRED)
find_package(JNI)

add_subdirectory("${gtest_dir}" "googletest")
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(LibJniPP_Tests_run
        ConvertersTests.cpp
        )

target_link_libraries(LibJniPP_Tests_run gtest gtest_main jni++_static ${JAVA_JVM_LIBRARY})