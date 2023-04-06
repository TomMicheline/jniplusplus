# libjni++ native code

## Building

You will either need `cmake` and do something like this:
```shell
        # cd <install location>/jniplusplus/native
        # mkdir cmake-build-cl
        # cd cmake-build-cl
        # cmake -S .. -B .
        # make install
```

Or you can also use `CLion` (which has it's own copy of cmake) by opening the `native` directory 
as a project.  You can then select `Install` from the Build menu.

In either case you should end up with the directory `native/build` with top level `lib`, `include`,
and `bin` directories.  If everything worked, you should be able to execute 