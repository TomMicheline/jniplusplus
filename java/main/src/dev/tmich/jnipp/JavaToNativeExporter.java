//
// dev.tmich.jnipp.JavaToNativeExporter.java
// libjni++
//
// Created by Thomas Micheline Jun 22, 2013.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
// Copyright © 2023 VSee Lab, Inc. All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

package dev.tmich.jnipp;

import dev.tmich.jnipp.logger.JniLogger;
import dev.tmich.jnipp.logger.JniPrintStreamLogger;

import java.lang.annotation.Annotation;
import java.lang.reflect.*;
import java.util.HashMap;
import java.util.Map;
import java.util.TreeMap;


//
// Java-land support code for calling Java from native code.
//
@SuppressWarnings("unused")  // Library api, some things will only ever be used in other projects
public class JavaToNativeExporter {
    // Configure logging
    private static JniLogger log = new JniPrintStreamLogger();
    public static void setLog(JniLogger log) {
        JavaToNativeExporter.log = log;
    }

    // Configure packages which do or don't require ExportToNative.  By default, classes and
    // their members do not require it.  If you configure a package here, either to require
    // or not require annotation, that package and subpackages follow that rule.  The longest
    // matching package wins.  So if you require annotations on com.example but don't require
    // annotations on com.example.generated, then com.example.backend.parsers.JSon would
    // require annotation to be exportable but com.example.generated.swig.NativeClass would not.
    private static final Map<String, Boolean> sAnnotationRequiredPackages = new HashMap<>();
    public static void setRequiresExport(String packageName, boolean isAnnotationRequired) {
        if (packageName == null) {
            return;
        }
        synchronized (sAnnotationRequiredPackages) {
            sAnnotationRequiredPackages.put(packageName, isAnnotationRequired);
        }
    }
    public static boolean doesPackageRequireExport(String packageName) {
        // Not really clear how this should be handled so revert to default behavior
        if (packageName == null) {
            return false;
        }

        String currentLongest = null;
        Boolean currentRequired = false;  // Defaults to false so that packages with no configuration do not require export
        synchronized (sAnnotationRequiredPackages) {
            for (Map.Entry<String, Boolean> entry : sAnnotationRequiredPackages.entrySet()) {
                String configuredPackage = entry.getKey();
                if (packageMatches(packageName, configuredPackage)) {
                    if (currentLongest == null || currentLongest.length() < configuredPackage.length()) {
                        currentLongest = configuredPackage;
                        currentRequired = entry.getValue();
                    }
                }
            }
        }
        return currentRequired;
    }
    private static boolean packageMatches(String packageName, String possibleMatchPackage) {
        if (packageName.equals(possibleMatchPackage)) {
            return true;
        }
        return packageName.startsWith(possibleMatchPackage + ".");
    }

    //
    // Does the element have the ExportToNative annotations?
    //
    private static boolean hasExportToNative(AnnotatedElement element) {
        Annotation[] annotations = element.getAnnotations();
        for (Annotation annotation : annotations) {
            if (annotation instanceof ExportToNative) {
                return true;
            }
        }
        return false;
    }

    //
    // Can the constructor, method, or field be exported to native code
    //
    @SuppressWarnings("BooleanMethodIsAlwaysInverted")
    private static boolean isExportable(Member member) {
        Class<?> cls = member.getDeclaringClass();

        // This class must always be exportable
        if (cls.equals(JavaToNativeExporter.class)) {
            return true;
        }

        // Check to see if the package is required to have ExportToNative.  By default,
        // they do not so that you can export the SDK or 3rd party libraries.
        if (!doesPackageRequireExport(cls.getPackageName())) {
            return true;
        }

        // First check to see if the entire class is exported
        do {
            if (hasExportToNative(cls)) {
                return true;
            }
            cls = cls.getDeclaringClass();
        } while (cls != null);

        // Otherwise check to see if it is annotated
        return hasExportToNative((AnnotatedElement) member);
    }

    //
    // Called from native code when method signature is not specified and search must be
    // done by name alone.  Valid matches must have the given name, and it may also need to
    // be annotated with ExportToNative.  There must be one (and only one) valid match.
    //
    @ExportToNative
    private static AccessibleObject lookupJavaMember(String className, String methodName, boolean isStatic, int numParameters) {
        boolean surpressLog = className.equals("dev.tmich.jnipp.JavaToNativeExporter") && methodName.equals("setMinimumLogLevel");
        String fullMethodName = className + "." + methodName;
        if (!surpressLog) log.debug("lookupJavaMember %s", fullMethodName);
        Class<?> cls;
        try {
            cls = Class.forName(className);
            if (!surpressLog) log.debug("Loaded class %s", cls.getCanonicalName());
        } catch (ClassNotFoundException e) {
            log.error("ClassNotFoundException %s", e.getMessage());
            e.printStackTrace();
            throw new RuntimeException(e);
        }

        Package pkg = cls.getPackage();

        if (methodName.equals("<init>")) {
            Constructor<?> foundConstructor = null;
            Constructor<?>[] allConstructors = cls.getDeclaredConstructors();
            for (Constructor<?> currentConstructor : allConstructors) {
                int currentNumParameters = currentConstructor.getParameterTypes().length;
                if (currentNumParameters != numParameters) {
                    log.debug("Constructor for '%s' but has the wrong number of parameters (%d != %d) [%s]", className, currentNumParameters, numParameters, currentConstructor.toString());
                    continue;
                }

                // If export is required, and it is not annotated, skip it.
                if (!isExportable(currentConstructor)) {
                    log.debug("Constructor found in '%s' but missing ExportToNative", className);
                    continue;
                }

                // We found a matching method.  However, if this isn't the first one we found error out.
                if (foundConstructor != null) {
                    log.error("lookupJavaMember: More than one constructor matches for %s!  Skipping!", className);
                    return null;
                }

                foundConstructor = currentConstructor;
            }

            if (foundConstructor == null) {
                log.debug("lookupJavaMember : Constructor not found for %s", className);
            } else {
                log.debug("Cache constructor for : %s", className);
            }

            return foundConstructor;
        } else {
            Map<String, Method> allMethods = new TreeMap<>();

            //
            // Blergh.  getMethods does what you'd think, basically.  It returns all the methods of the
            // current class and its superclasses.  But only the public ones.  getDeclaredMethods, on the
            // other hand, gets methods with all visibilities but only those implemented in the current
            // class, not those inherited.  So, use getDeclaredMethods on each class and filter out
            // the overridden methods.
            //
            Class<?> currentClass = cls;
            do {
                for (Method m : currentClass.getDeclaredMethods()) {
                    if (!m.getName().replace("$app_debug", "").equals(methodName)) continue;
                    String key = makeKey(m);
                    if (!allMethods.containsKey(key))
                        allMethods.put(key, m);
                }
                currentClass = currentClass.getSuperclass();
            } while (currentClass != null);

            Method foundMethod = null;
            for (Method currentMethod : allMethods.values()) {

                // Check static vs. instance
                if (Modifier.isStatic(currentMethod.getModifiers()) != isStatic) {
                    //if (!surpressLog) log.debug("Static (%b) doesn't match for %s", isStatic, currentMethod.toString());
                    continue;
                }

                // Correct number of parameters?
                int currentNumParameters = currentMethod.getParameterTypes().length;
                if (currentNumParameters != numParameters) {
                    //if (!surpressLog) log.debug("Wrong number of parameters (%d != %d) for ", currentNumParameters, numParameters, currentMethod.toString());
                    continue;
                }

                // If this class is in an export required package, and it is not annotated, skip it.
                if (!isExportable(currentMethod)) {
                    //if (!surpressLog) log.debug("Missing ExportToNative on %s", currentMethod.toString());
                    continue;
                }

                // We found a matching method.  However, if this isn't the first one we found error out.
                if (foundMethod != null) {
                    log.error("lookupJavaMember: Both %s and %s match!  Skipping!", foundMethod.toString(), currentMethod.toString());
                    return null;
                }

                foundMethod = currentMethod;
            }

            if (foundMethod == null) {
                log.error("lookupJavaMember : Method not found %s", fullMethodName);
            } else {
                if (!surpressLog) log.debug("Cache method: %s", foundMethod.toString());
            }

            return foundMethod;
        }
    }

    //
    // Called from native code when field signature is not specified and search must be
    // done by name alone.  Valid matches must have the given name and, if the class is in
    // an export required package, it must also be annotated with ExportToNative.
    //
    @ExportToNative
    private static Field lookupJavaField(String className, String fieldName, boolean isStatic) {
        String fullFieldName = className + "." + fieldName;
        log.debug("lookupJavaField %s", fullFieldName);
        Class<?> cls;
        try {
            cls = Class.forName(className);
            log.debug("Loaded class %s", cls);
        } catch (ClassNotFoundException e) {
            log.debug("ClassNotFoundException %s", e.getMessage());
            e.printStackTrace();
            throw new RuntimeException(e);
        }

        Class<?> currentClass = cls;
        Field foundField = null;

        do {
            Field[] allFields = currentClass.getDeclaredFields();

            for (Field currentField : allFields) {
                if (currentField.getName().equals(fieldName)) {
                    foundField = currentField;
                    break;
                }
            }

            currentClass = currentClass.getSuperclass();

        } while (currentClass != null && foundField == null);

        if (foundField == null) {
            log.debug("lookupJavaMember : Field not found %s.%s", className, fieldName);
        } else if (!isExportable(foundField)) {
            log.debug("lookupJavaMember : Field found but is not exported: %s.%s", className, fieldName);
            foundField = null;
        } else {
            log.debug("Cache field : %s.%s", className, fieldName);
        }

        return foundField;
    }

    private static String makeKey(Method m) {
        String desc = m.toString();
        String key = desc.substring(desc.indexOf(m.getName()));
        if (key.contains(") throws")) {
            key = key.substring(0, key.lastIndexOf(") throws") + 1);
        }
        return key;
    }

    //
    // Called from native code when method signature is specified to make sure that, if the
    // class is in an export required package, it (or the class) has the ExportToNative annotation.
    //
    @ExportToNative
    private static boolean javaMemberExported(AccessibleObject member) {
        Class<?> cls = null;
        String name = null;
        if (member instanceof Method) {
            cls = ((Method) member).getDeclaringClass();
            name = ((Method) member).getName();
        } else if (member instanceof Constructor) {
            cls = ((Constructor<?>) member).getDeclaringClass();
            name = ((Constructor<?>) member).getName();
        } else if (member instanceof Field) {
            cls = ((Field) member).getDeclaringClass();
            name = ((Field) member).getName();
        }
        assert cls != null;

        boolean classHasAnnotation = hasExportToNative(cls);
        boolean memberHasAnnotation = hasExportToNative(member);
        log.debug("checking method %s of class %s classHasAnnotation=%b methodHasAnnotation=%b",
            name, cls.getCanonicalName(), classHasAnnotation, memberHasAnnotation);
        return classHasAnnotation || memberHasAnnotation;
    }

    public static void registerSingleton(Object obj) {
        registerSingleton(obj.getClass().getName(), obj);
    }

    public static void unregisterSingleton(Object obj) {
        unregisterSingleton(obj.getClass().getName(), obj);
    }

    public native static void unregisterAllSingletons();

    private native static void registerSingleton(String name, Object obj);
    private native static void unregisterSingleton(String name, Object obj);

    //
    // Mechanism to pass logging back to native logger.
    //
    public native static void nativeLog(int level, String msg);

    private static JniLogger.LogLevel gMinimumLogLevel = JniLogger.LogLevel.Debug;
    public static void setMinimumLogLevel(int levelOrd) {
        gMinimumLogLevel = levelOrdinalToLogLevel(levelOrd);
    }
    public static JniLogger.LogLevel getgMinimumLogLevel() {
        return gMinimumLogLevel;
    }

    //
    // And the inverse, pass native logs to JVM
    //
    public static void jvmLog(int levelOrd, String msg) {
        switch (levelOrdinalToLogLevel(levelOrd)) {
            case Debug:
                log.debug(msg);
                break;
            case Warning:
                log.warning(msg);
                break;
            case Error:
                log.error(msg);
                break;
        }
    }

    private static JniLogger.LogLevel levelOrdinalToLogLevel(int levelOrd) {
        levelOrd = Math.max(0, levelOrd);
        levelOrd = Math.min(levelOrd, JniLogger.LogLevel.values().length-1);
        return JniLogger.LogLevel.values()[levelOrd];
    }

    //
    // Support for adding JVM classloader pure native threads.  When a thread is created in native code we first
    // have it call bindToAppClassLoader, which then calls back into native code and then nativeThreadWrapper
    // invokes the original target function for the thread.  This has the effect of associating a Java
    // class loader with the thread.
    //
    public static native int nativeThreadWrapper(long pointerToCallbackData);

    @ExportToNative
    public static int bindToAppClassLoader(String name, long pointerToCallbackData) {
        Thread.currentThread().setName(name);
        return nativeThreadWrapper(pointerToCallbackData);
    }
}
