//
// build.gradle.kts
// jni++
//
// Created by Thomas Micheline Mar 26, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
// Copyright © 2023 VSee Lab, Inc. All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

plugins {
    id("java-library")
}

tasks.jar {
    archiveFileName.set("jni++.jar")
}

group = "dev.tmich"
version = "1.0"

dependencies {
    testImplementation("org.junit.jupiter:junit-jupiter-api:5.8.2")
    testRuntimeOnly("org.junit.jupiter:junit-jupiter-engine:5.8.2")
}

tasks.getByName<Test>("test") {
    useJUnitPlatform()
}

tasks.register<Copy>("copyJar") {
    from(tasks.jar)
    into("../install/lib")
}

tasks.build {
  dependsOn("copyJar")
}
java {
    sourceCompatibility = JavaVersion.VERSION_1_8
    targetCompatibility = JavaVersion.VERSION_1_8
}