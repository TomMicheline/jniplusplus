# jni++ native code

## Developing

To open the native code project in CLion just open the `native_project` directory as a project.
CLion will load the CMakeLists.txt file in that directory and use it to find all the necessary
paths.

## Building

You will either need `cmake` and do something like this:
```shell
        # cd <install location>/jniplusplus/native_project
        # mkdir cmake-build-cl
        # cd cmake-build-cl
        # cmake -S .. -B .
        # make install
```

Or you can also use `CLion` (which has its own copy of cmake) by selecting `Install` from 
the Build menu.

## Running the simple example app

In either case you should end up with the directory `jniplusplus/install` with top level `lib`, 
`include`, and `bin` directories.  If everything worked, you should be able to execute 
build/bin/simpleapp.  You will need to pass the path to `jni++.jar` on the command line:
```shell
# cd <install location>/jniplusplus
# install/bin/simpleapp install/lib/jni++.jar
Is this upper case? 'HELLO, WORLD.'
[Plain JNI] Does the pattern match text #1? YES
[Plain JNI] Does the pattern match text #2? NO
Does the pattern match text #1? YES
Does the pattern match text #2? NO
```


