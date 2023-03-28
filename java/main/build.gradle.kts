//
// build.gradle.kts
// libjni++
//
// Created by Thomas Micheline Mar 26, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
// Copyright © 2023 VSee Lab, Inc. All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

plugins {
    id("java")
}

sourceSets {
    main {
        java {
            setSrcDirs(listOf("src"))
        }
    }
}

tasks.jar {
    archiveFileName.set("libjni++.jar")
}

group = "dev.tmich"
version = "1.0"

repositories {
    mavenCentral()
}

dependencies {
    testImplementation("org.junit.jupiter:junit-jupiter-api:5.8.1")
    testRuntimeOnly("org.junit.jupiter:junit-jupiter-engine:5.8.1")
}

tasks.getByName<Test>("test") {
    useJUnitPlatform()
}
