//
// build.rs
// jni++
//
// Created by Thomas Micheline Apr 10, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

extern crate bindgen;

use std::env;

fn main() {
    let java_home = env::var("JAVA_HOME").unwrap();

    println!("cargo:rerun-if-changed=src/matchers.h");
    println!("cargo:rerun-if-changed=src/matchers.cpp");

    println!(r"cargo:rustc-link-search=../../install/lib");
    println!(r"cargo:rustc-link-search={}/lib/server", java_home);

    println!(r"cargo:rustc-link-lib=jni++_static");
    println!(r"cargo:rustc-link-lib=jvm");

    println!(r"cargo:rustc-link-arg=-Wl,-rpath,{}/lib/server", java_home);

    let mut builder = cc::Build::new();
    let build = builder
        .cpp(true)
        .file("src/matchers.cpp")
        .include("src")
        .include(format!("{}/include", java_home))
        .include(format!("{}/include/darwin", java_home))
        .include(format!("{}/include/linux", java_home))
        .include(format!("{}/include/windows", java_home))
        .include("../../install/include")
        .flag("-std=c++20")
        .flag("-Wno-unused-parameter")
        .cpp_link_stdlib("c++");

        build.compile("matchers");

    let bindings = bindgen::Builder::default()
        .header("src/matchers.h")
        .clang_arg(format!("-I{}/include", java_home))
        .clang_arg(format!("-I{}/include/darwin", java_home))
        .clang_arg(format!("-I{}/include/linux", java_home))
        .clang_arg(format!("-I{}/include/windows", java_home))
        .generate()
        .expect("Unable to generate bindings");

    bindings
        .write_to_file("src/bindings.rs")
        .expect("Couldn't write bindings!");
}