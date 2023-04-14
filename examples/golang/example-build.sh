#!/usr/bin/env bash

##
## This script just tries to figure out the correct build paths for go including
## JNI and jni++
##

SRC_DIR="$(realpath "$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")")"
BASE_DIRDIR="$(realpath $SRC_DIR/../..)"
INSTALL_DIR="${BASE_DIRDIR}/install"
JAR_PATH="${INSTALL_DIR}/lib/jni++.jar"

if [ ! -f $INSTALL_DIR/include/JniPlusPlus.hpp ] || [ ! -f $INSTALL_DIR/lib/libjni++_static.a ] || [ ! -f $JAR_PATH ]
then
  echo "jni++ must be built and installed in the local project directory before the "
  echo "golang experiment can be built.  To build it do:"
  echo "# cd <installed directory>/jniplusplus/native_project"
  echo "# mkdir cmake-build-cl"
  echo "# cd cmake-build-cl"
  echo "# cmake -S .. -B ."
  echo "# make install"
  echo "# cd ../java_project"
  echo "# ./gradlew :jni++:build"
  exit 1
fi

if [ -z "$JAVA_HOME" ]
then
  if command -v java &> /dev/null
  then
    JAVA_HOME="$(java -XshowSettings:properties -version 2>&1 > /dev/null | grep 'java.home' | sed 's,.* = ,,')"
  fi
fi

if [ -z "$JAVA_HOME" ]
then
  echo "Please set JAVA_HOME.  Exiting..."
  exit 1
fi

JAVA_INCLUDE="$JAVA_HOME/include"

if [ ! -f "$JAVA_INCLUDE/jni.h" ]
then
  echo "Can not find jni.h in JAVA_HOME (${JAVA_HOME}).  Exiting..."
  exit 1
fi

if [ -z "$JAVA_PLATFORM" ]
then
  JAVA_PLATFORM="$(uname -s | tr '[:upper:]' '[:lower:]')"
fi

JAVA_PLATFORM_INCLUDE="$JAVA_INCLUDE/$JAVA_PLATFORM"

if [ ! -f "$JAVA_PLATFORM_INCLUDE/jni_md.h" ]
then
  echo "Can not find jni_md.h in ${JAVA_PLATFORM_INCLUDE}."
  eccho "Please set JAVA_PLATFORM (probably linux, darwin, etc).  Exiting..."
  exit 1
fi

JAVA_INCLUDE_OPTIONS="-I$JAVA_INCLUDE -I$JAVA_PLATFORM_INCLUDE"
JVM_LIB_DIR="$JAVA_HOME/lib/server"

CGO_CFLAGS="-std=gnu17 $JAVA_INCLUDE_OPTIONS" \
CGO_CXXFLAGS="-std=c++20 $JAVA_INCLUDE_OPTIONS -I$INSTALL_DIR/include" \
CGO_LDFLAGS="-L$INSTALL_DIR/lib -ljni++_static -L$JVM_LIB_DIR -ljvm -Wl,-rpath,$JVM_LIB_DIR" \
go build  -ldflags "-X main.JarPath=$JAR_PATH" "$@" .
