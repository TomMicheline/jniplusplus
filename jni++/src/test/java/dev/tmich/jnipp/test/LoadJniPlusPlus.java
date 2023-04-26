package dev.tmich.jnipp.test;

import dev.tmich.jnipp.JavaToNativeExporter;
import dev.tmich.jnipp.logger.JniLogger;

public class LoadJniPlusPlus {
    public static void DoLoad() {
        JniLogger logger = JavaToNativeExporter.getLog();
        try {
            System.loadLibrary("jniplusplus");
            logger.debug("Successfully loaded jniplusplus statically linked library.");
        } catch (Exception e) {
            logger.error("Error loading jniplusplus statically linked library.");
        }
    }
}
