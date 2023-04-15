group = "dev.tmich"

tasks.register<Delete>("clean") {
    delete(rootProject.buildDir)
}