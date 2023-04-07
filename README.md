# libjni++ : C++ JNI without the boilerplate 

Modern c++ library for making JVM calls via JNI using variadic templates to reduce boilerplate, 
leverage automatic type conversion, and enable compile time type checking.

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

First, lets see what this would look like using just plain JNI.  The only part of the libjni++ library 
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

Next, lets see what it looks like using libjni++:

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

Not only is the libjni++ code much shorter, it is easier to write and to understand.  When
writing the plain JNI section, several errors were introduced in the signatures which can be tricky to 
get right.  In particular, I didn't notice that `Pattern.matcher()` takes a `CharSequence` not a `String` which
wasted time in the debugger figuring out which method lookup failed and why.

Both of these examples are pulled directly from the simple example app included in `native/examples/simple_app`.

## Build

Currently, the C++ code uses cmake and the Java code uses gradle.  Eventually, the build systems will 
be unified, probably to gradle.  Development is done using CLion for C++ code and IntelliJ IDEA for 
Java code.  

Check the README.md file in the `java` and `native` directories for information on how to build each.

## Examples

There are a couple of examples under `native/examples` to illustrate how libjni++ can be used and
experiment with either calling libjni++ code from other languages or calling into other JVM 
languages.  It is a work-in-progress.  For `simpleapp`, unless your current working directory
is `native/<cmake build dir>/examples/simple_app` you will probably need to pass the path to the
`libjni++.jar` file on the command line.   For example, if you are in the `native` directory you 
might do:
```shell
# pwd
/home/parallels/Projects/jniplusplus/native
# ./build/bin/simpleapp ../java/main/build/libs/libjni++.jar 
Is this upper case? 'HELLO, WORLD.'
[Plain JNI] Does the pattern match text #1? YES
[Plain JNI] Does the pattern match text #2? NO
Does the pattern match text #1? YES
Does the pattern match text #2? NO
# 
```

