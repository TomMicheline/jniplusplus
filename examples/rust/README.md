# Rust Example Application

This is an experiment to see how libjni++ works when called from Rust using bindgen. 

## Prerequisites

1. Build Java library used by libjni++.  The gradle based build is in jniplusplus/java_project.
   You can build it on the command line like `./gradlew :libjni++:build` or use an IDE that
   supports gradle based projects like JetBrains's IDEA and execute the same task.
2. Build the native libjni++ library.  You will need `cmake` and one way to do it is
   like this:
```shell
        # cd <install location>/jniplusplus/native_project
        # mkdir cmake-build-cl
        # cd cmake-build-cl
        # cmake -S .. -B .
        # make install
```
3. You need to set the JAVA_HOME environment variable if you don't have it set
   already.
4. Build and run the app with `cargo run`
6. If everything worked, you should see something like:
```shell
# cargo run
   Compiling libc v0.2.141
   Compiling memchr v2.5.0
   <-- snip a bunch of similar lines -->
   Compiling cexpr v0.6.0
   Compiling rust v0.1.0 (/Users/tom/Projects/jniplusplus/examples/rust)
    Finished dev [unoptimized + debuginfo] target(s) in 6.81s
     Running `target/debug/rust`
Matches #1: true
Matches #2: false
```

## Building and running in an IDE

I had pretty good luck opening the "rust" directory in CLion with the Rust plugin installed.  I had to 
click the link on two alerts to fixes environment issues, which I assume are at least partially due to the
fact that I'm not a Rust developer and probably don't have my environment setup quite right.  The two issues
were setting the rust toolchain location and attaching the Cargo build.  Once I fixed those two issues, everything
worked.

The easiest way to create a run configuration to run, debug, or build the app is to click the little run gutter
icon in main.rs next to the main function.  Once it is created you can just run or build it from the controls at 
the top.
