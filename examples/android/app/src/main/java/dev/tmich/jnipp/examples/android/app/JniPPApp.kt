package dev.tmich.jnipp.examples.android.app

import android.app.Application
import android.util.Log

import dev.tmich.jnipp.android.AndroidLogger;
import dev.tmich.jnipp.JavaToNativeExporter;

const val TAG = "jni++"

class JniPPApp : Application() {
    override fun onCreate() {
        super.onCreate()
        JavaToNativeExporter.setLog(AndroidLogger())
        loadNative()
    }

    private fun loadNative() =
        try {
            System.loadLibrary("JniPPExample")
            Log.d(TAG, "Native library loaded")
            true
        } catch (e: SecurityException) {
            Log.e(TAG, "Loading native library not allowed by security manager: ", e)
            false
        } catch (e: UnsatisfiedLinkError) {
            Log.e(TAG, "Native library not found: ", e)
            false
        }
}