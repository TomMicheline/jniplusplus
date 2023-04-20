project(jni++_tests)

find_package(Java REQUIRED)
find_package(JNI)

add_subdirectory("${gtest_dir}" "googletest")
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Remove old wrappers before re-running SWIG.  Dependency is added below.
add_custom_target(
        CleanupSwig
        COMMAND ${CMAKE_COMMAND} -E rm -rf swig-java
)

add_executable(LibJniPP_Tests_run
        BoxedTests.cpp
        ConvertersTests.cpp
        JniMappingTests.cpp
        PrimitivesTests.cpp
        TestClass.cpp
        TestClass.hpp
        Test.i
        )


# Fail the build if SWIG is not found - Ensure SWIG is installed and on the system path
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

# Ensure file recognized as C++ (otherwise, exported as C file)
set_property(SOURCE Test.i PROPERTY CPLUSPLUS ON)

# Setup SWIG flags and locations
set(CMAKE_SWIG_FLAGS -c++ -package "dev.tmich.jnipp.test.swig")

# Export wrapper files to Java, and link with the created C++ library
swig_add_library(
        testswig
        TYPE STATIC
        LANGUAGE java
        SOURCES Test.i
        OUTPUT_DIR swig-java
        OUTFILE_DIR swig-cxx
)

target_include_directories(testswig PUBLIC ${JNI_INCLUDE_DIRS} ".")

# Make sure any old wrappers are cleaned up before SWIG is run again
add_dependencies(
        testswig
        CleanupSwig
)

# Manually manage dependencies for now
set_property(
        TARGET testswig
        PROPERTY SWIG_DEPENDS
        TestClass.hpp
)

target_link_libraries(LibJniPP_Tests_run gtest gtest_main jni++_static testswig ${JAVA_JVM_LIBRARY})

# After SWIG has generated the new wrappers, remove the old versions (in case there are any
# that don't exist anymore) and then copy the new ones over.
add_custom_command(
        TARGET testswig POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Removing old java files"
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_CURRENT_LIST_DIR}/../java/dev/tmich/jnipp/test/swig
        COMMAND ${CMAKE_COMMAND} -E echo "(Re)creating swig dir"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_LIST_DIR}/../java/dev/tmich/jnipp/test/swig
        COMMAND ${CMAKE_COMMAND} -E echo "Copying swig java files"
        COMMAND ${CMAKE_COMMAND} -E copy_directory swig-java ${CMAKE_CURRENT_LIST_DIR}/../java/dev/tmich/jnipp/test/swig
        VERBATIM
)