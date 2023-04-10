# libjni++ Java source files

## Developing

To open the java code project in IDEA just open the `native_project` directory as a project.
IDEA will load the gradle build in that directory and use it to find all the necessary
paths.  It is setup as a composite build.

## Build

The java sources are built using gradle.   You can either build it on the command 
line like `./gradlew :libjni++:build` or use an IDE that supports gradle based projects like 
JetBrains's IDEA.  However, just building the project in IDEA will not create the jar file
nor install it.  You need to run the :libjni++:build target for that.
