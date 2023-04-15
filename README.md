# jni++ : C++ JNI without the boilerplate 

Modern c++ library for making JVM calls via JNI using variadic templates to reduce boilerplate, 
leverage automatic type conversion, and enable compile-time type checking.

More information can be found at the [project website](https://jniplusplus.micheline.dev/).

## Illustration by example

Let's say we want to call some regex Java code from C++ (I know, it's silly, but it is just an example).
We can recreate the basic functionality of the following Java code:

```java
    Pattern pattern = Pattern.compile(".*JNI.*rocks.*");
    Matcher matcher1 = pattern.matcher("This strings will see if JNI is dumb as rocks or not");
    Matcher matcher2 = pattern.matcher("This strings will see if jni is dumb as rocks or not");
    System.out.println("Does the pattern match text #1? %s", matcher1.matches() ? "YES" : "NO");
    System.out.println("Does the pattern match text #2? %s", matcher2.matches() ? "YES" : "NO");
```

First, lets see what this would look like using just plain JNI.  The only part of the jni++ library 
used in this code is `env()`.

```c++
    jclass patternClass = env()->FindClass("java/util/regex/Pattern");
    jclass matcherClass = env()->FindClass("java/util/regex/Matcher");
    if (!patternClass || !matcherClass) {
        getLogger()->error("SimpleApp: Could not get JNI classes for plain JNI example.");
        return -1;
    }
    
    jmethodID compileMethod = env()->GetStaticMethodID(patternClass, "compile", "(Ljava/lang/String;)Ljava/util/regex/Pattern;");
    jmethodID matcherMethod = env()->GetMethodID(patternClass, "matcher", "(Ljava/lang/CharSequence;)Ljava/util/regex/Matcher;");
    jmethodID matchesMethod = env()->GetMethodID(matcherClass, "matches", "()Z");
    if (!compileMethod || !matcherMethod || !matchesMethod) {
        getLogger()->error("SimpleApp: Could not get JNI methods for plain JNI example.");
        return -1;
    }
    
    jstring patternString = env()->NewStringUTF(".*JNI.*rocks.*");
    jobject patternObject = env()->CallStaticObjectMethod(patternClass, compileMethod, patternString);
    jstring matchingString1 = env()->NewStringUTF("This strings will see if JNI is dumb as rocks or not");
    jobject matcher1Object = env()->CallObjectMethod(patternObject, matcherMethod, matchingString1);
    jstring matchingString2 = env()->NewStringUTF("This strings will see if jni is dumb as rocks or not");
    jobject matcher2Object = env()->CallObjectMethod(patternObject, matcherMethod, matchingString2);
    
    jboolean result1 = env()->CallBooleanMethod(matcher1Object, matchesMethod);
    jboolean result2 = env()->CallBooleanMethod(matcher2Object, matchesMethod);
    
    std::cout << "[Plain JNI] Does the pattern match text #1? " << (result1 ? "YES" : "NO") << std::endl;
    std::cout << "[Plain JNI] Does the pattern match text #2? " << (result2 ? "YES" : "NO") << std::endl;
```

Next, lets see what it looks like using jni++:

```c++
    StaticMethod<jlocal, const std::string&> jCompilePattern("java.util.regex.Pattern", "compile");
    InstanceMethod<jlocal, const std::string&> jPatternMatcher("java.util.regex.Pattern", "matcher");
    InstanceMethod<bool> jMatcherMatches("java.util.regex.Matcher", "matches");
    
    auto pattern = jCompilePattern(".*JNI.*rocks.*");
    auto matcher1 = jPatternMatcher(pattern, "This strings will see if JNI is dumb as rocks or not");
    auto matcher2 = jPatternMatcher(pattern, "This strings will see if jni is dumb as rocks or not");
    
    std::cout << "Does the pattern match text #1? " << (jMatcherMatches(matcher1) ? "YES" : "NO") << std::endl;
    std::cout << "Does the pattern match text #2? " << (jMatcherMatches(matcher2) ? "YES" : "NO") << std::endl;
```

Not only is the jni++ code much shorter, it is easier to write and to understand.  When
writing the plain JNI section, several errors were introduced in the signatures which can be tricky to 
get right.  In particular, I didn't notice that `Pattern.matcher()` takes a `CharSequence` not a `String` which
wasted time in the debugger figuring out which method lookup failed and why.

Both of these examples are pulled directly from the simple example app included in `examples/simple_app`.

## Build

First, make sure you have the submodules checked out.  If you did not use --recursive (or similar) on
your clone you need to get the submodules with something like:
```shell
# git submodule update --init --recursive
Submodule 'extern/doxygen-awesome-css' (https://github.com/jothepro/doxygen-awesome-css.git) registered for path 'extern/doxygen-awesome-css'
Submodule 'extern/googletest' (https://github.com/google/googletest.git) registered for path 'extern/googletest'
Cloning into '/home/vagrant/jniplusplus/extern/doxygen-awesome-css'...
Cloning into '/home/vagrant/jniplusplus/extern/googletest'...
Submodule path 'extern/doxygen-awesome-css': checked out 'a7f7891706c656903326f79baf74beb2b711688d'
Submodule path 'extern/googletest': checked out 'b796f7d44681514f58a683a3a71ff17c94edb0c1'
#
```
Currently, the C++ code uses cmake and the Java code uses gradle.  Eventually, the build systems will 
be unified, probably to gradle.  However, CLion gradle support seems to be a work in progress and there
don't seem to be any plans to enable using the gradle native build plugins for in the AGP.  

I use both CLion and IDEA to edit the two parts of the code.  Using different IDEs for the native and 
java code causes its own friction above and beyond that caused by two different build systems.  Both IDEs 
use the same project files, so they collide.  To solve this there is a project folder (`native_project`) 
which links to the necessary parts of the native build.  The Java build files are in the top level folder.

### Building the Java library

You can either build it on the command  line like `./gradlew :jni++:build` or use an IDE that supports 
gradle based projects like JetBrains's IDEA.  However, just building the project in IDEA will not create 
the jar file nor install it.  You need to run the :jni++:build target for that.

### Building the native library

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

## Deveoping / IDE

### Using Intellij IDEA to edit the JVM code

To open the java code project in IDEA just open the `jniplusplus` directory (or the `jniplusplus/settings.gradle.kts`
file) as a project.  IDEA will load the gradle build in that directory and use it to find all the necessary
paths.

### Using CLion to edit the C++ code

To open the native code project in CLion just open the `native_project` directory as a project.
CLion will load the CMakeLists.txt file in that directory and use it to find all the necessary
paths.

### Another option (Android Studio)

Android Studio allows you to edit and build both C++ and Java code but it really works best with an Android project.
There is, however, an Android example project in `examples/android` that links to the jni++ libraries in such a way that
it is pretty convenient to edit both even if you aren't interested in the App code itself.

## Examples

There are a couple of examples in the `examples` directory to illustrate how jni++ can be used and
experiment with either calling jni++ code from other languages (currently rust and go) or calling into other JVM 
languages (currently just Kotlin in the `android` example but more to come).  It is a work-in-progress.  

If you followed the build steps above, you should end up with the directory `jniplusplus/install` with top level `lib`,
`include`, and `bin` directories.  Assuming that all worked you can test it by running `install/bin/simpleapp` (source
code in `examples/simple_app`) which will use both standard JNI calls and jni++ to do the same, simple calls into the
JDK regex library. You will need to pass the path to `jni++.jar` on the command line:

```shell
# pwd
/home/parallels/Projects/jniplusplus
# ./install/bin/simpleapp install/lib/jni++.jar 
Is this upper case? 'HELLO, WORLD.'
[Plain JNI] Does the pattern match text #1? YES
[Plain JNI] Does the pattern match text #2? NO
Does the pattern match text #1? YES
Does the pattern match text #2? NO
# 
```
