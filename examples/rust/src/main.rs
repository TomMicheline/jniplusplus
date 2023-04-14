//
// main.rs
// jni++
//
// Created by Thomas Micheline Apr 10, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

use std::ffi::{CString, NulError};

include!("bindings.rs");

fn main() -> Result<(), NulError> {
    unsafe {
        if !setupJava(CString::new("../../install/lib/jni++.jar" )?.as_ptr()) {
            panic!("Failed to setup JNI: bailing!");
        }
        let pattern = javaCompilePattern( CString::new( ".*JNI.*rocks.*")?.as_ptr() );
        println!("Matches #1: {}", javaPatternMatches(pattern, CString::new("This strings will see if JNI is dumb as rocks or not")?.as_ptr()));
        println!("Matches #2: {}", javaPatternMatches(pattern, CString::new("This strings will see if jni is dumb as rocks or not")?.as_ptr()));
    }
    Ok(())
}
