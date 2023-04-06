# Go Example Application

This is an experiment to see how libjni++ works when called from Go using cgo.  This
probably wouldn't make sense in a production environment but there might be use-cases
where it would.  

## Issues

* Not really libjni++ specific but there is friction between c types and go types.
  There isn't really anything we can do about that but it is unfortunate because one
  of the main design goals of libjni++ is to reduce boilerplate and make calling JVM 
  methods as painless as possible.
* Because cgo can only directly wrap C functions and not C++ classes/functions it is 
  necessary to write a wrapper for any JVM calls to hide the templates from cgo.  In
  this experiment the function implementations are in c_wrappers.cpp and the declarations
  are in the import "C" preamble in main.go.

## Prerequisites

1. Build Java library used by libjni++.  The gradle based build is in jniplusplus/java.
   You can either build it on the command line like `./gradlew jar` or use an IDE that
   supports gradle based projects like JetBrains's IDEA.
2. Build the native libjni++ library.  You will need `cmake` and one way to do it is
   like this:
```shell
        # cd <install location>/jniplusplus/native
        # mkdir cmake-build-cl
        # cd cmake-build-cl
        # cmake -S .. -B .
        # make install
```
3. You might need to set the JAVA_HOME environment variable if you don't have it set 
   already.  The build script tries to figure it out if you don't but YMMV.
4. Run the build script, `example-build.sh`
5. If everything worked, run the executable `go_example`

## Building and running in an IDE

There are some environment variables and a go command line option set by 
`example-build.sh` that you will need to add to the run configuration in your 
IDE.  I've only done this in JetBrains's GoLand but there is likely to be 
similar controls in most IDEs.

1. You need to capture the values used by `example-build.sh`.  The easiest way to 
   do this is to run it with all commands echoed to the terminal.  Do this like this:
```shell
    # bash -x ./example-build.sh
       <snip out a bunch of output that isn't needed>
    + JVM_LIB_DIR=/Users/tom/.sdkman/candidates/java/current/lib/server
    + JAR_PATH=/Users/tom/Projects/jniplusplus/native/../java/main/build/libs/libjni++.jar
    + CGO_CFLAGS='-std=c17 -I/Users/tom/.sdkman/candidates/java/current/include -I/Users/tom/.sdkman/candidates/java/current/include/darwin'
    + CGO_CXXFLAGS='-std=c++20 -I/Users/tom/.sdkman/candidates/java/current/include -I/Users/tom/.sdkman/candidates/java/current/include/darwin -I/Users/tom/Projects/jniplusplus/native/build/include'
    + CGO_LDFLAGS='-L/Users/tom/Projects/jniplusplus/native/build/lib -ljni++_static -L/Users/tom/.sdkman/candidates/java/current/lib/server -ljvm -Wl,-rpath,/Users/tom/.sdkman/candidates/java/current/lib/server'
    + go build -ldflags '-X main.JarPath=/Users/tom/Projects/jniplusplus/native/../java/main/build/libs/libjni++.jar' .
```
2. The environment  variables you need to copy into your run configuration are the ones that 
   start with `CGO_`.
3. Copy the `-ldflags '-X...'` into the go tools command line options setting.  Alternatively
   you could pass the path to the libjni++.jar file to `go_example` as the first command line 
   argument.
