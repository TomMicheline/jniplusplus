package main

//
//#include <stdbool.h>
//#include "jni.h"
//
//bool setupJava(const char *jarPath);
//jobject javaCompilePattern(const char *regex);
//bool javaPatternMatches(jobject pattern, const char *testString);
//
import "C"

import (
	"fmt"
	"os"
)

var JarPath string

func main() {
	// Make it possible to supply a new path to jni++.jar
	if len(os.Args) > 1 {
		JarPath = os.Args[1]
	}

	if !fileExists(JarPath) {
		fmt.Printf("Error: Jar file '%v' does not exist!\n", JarPath)
		return
	}

	if !C.setupJava(C.CString(JarPath)) {
		fmt.Println("Error: Java setup failed!")
		return
	}

	pattern := C.javaCompilePattern(C.CString(".*JNI.*rocks.*"))
	matches1 := bool(C.javaPatternMatches(pattern, C.CString("This strings will see if JNI is dumb as rocks or not")))
	matches2 := bool(C.javaPatternMatches(pattern, C.CString("This strings will see if jni is dumb as rocks or not")))
	fmt.Printf("#1 Matches = %t\n", matches1)
	fmt.Printf("#2 Matches = %t\n", matches2)
}

func fileExists(filename string) bool {
	info, err := os.Stat(filename)
	if os.IsNotExist(err) {
		return false
	}
	return !info.IsDir()
}
