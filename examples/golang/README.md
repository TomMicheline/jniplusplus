# Go Example Application

This is an experiment to see how jni++ works when called from Go using cgo.  

## Prerequisites

1. Build Java library used by jni++.  The gradle based build is in jniplusplus/java_project.
   You can build it on the command line like `./gradlew :jni++:build` or use an IDE that
   supports gradle based projects like JetBrains's IDEA and execute the same task.
2. Build the native jni++ library.  You will need `cmake` and one way to do it is
   like this:
```shell
        # cd <install location>/jniplusplus/native_project
        # mkdir cmake-build-cl
        # cd cmake-build-cl
        # cmake -S .. -B .
        # make install
```
3. You might need to set the JAVA_HOME environment variable if you don't have it set 
   already.  The build script tries to figure it out if you don't but YMMV.
4. Initialize your go module with something like: `go mod init go_example`
5. Run the build script, `example-build.sh`
6. If everything worked, run the executable `go_example` (the executable will have the same 
name as your module).

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
    + JAR_PATH=/Users/tom/Projects/jniplusplus/native/../java/main/build/libs/jni++.jar
    + CGO_CFLAGS='-std=c17 -I/Users/tom/.sdkman/candidates/java/current/include -I/Users/tom/.sdkman/candidates/java/current/include/darwin'
    + CGO_CXXFLAGS='-std=c++20 -I/Users/tom/.sdkman/candidates/java/current/include -I/Users/tom/.sdkman/candidates/java/current/include/darwin -I/Users/tom/Projects/jniplusplus/native/build/include'
    + CGO_LDFLAGS='-L/Users/tom/Projects/jniplusplus/native/build/lib -ljni++_static -L/Users/tom/.sdkman/candidates/java/current/lib/server -ljvm -Wl,-rpath,/Users/tom/.sdkman/candidates/java/current/lib/server'
    + go build -ldflags '-X main.JarPath=/Users/tom/Projects/jniplusplus/native/../java/main/build/libs/jni++.jar' .
```
2. The environment  variables you need to copy into your run configuration are the ones that 
   start with `CGO_`.
3. Copy the `-ldflags '-X...'` into the go tools command line options setting.  Alternatively
   you could pass the path to the jni++.jar file to `go_example` as the first command line 
   argument.
