//
// dev.tmich.jnipp.test.TestStaticPrimitives.java
// jni++
//
// Created by Thomas Micheline Apr 19, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

package dev.tmich.jnipp.test;

public class TestStaticPrimitives {
    public static int getInt() { return 7; }
    public static int timesTwoInt(int inval) { return 2 * inval; }

    public static short getShort() { return 7; }
    public static short timesTwoShort(short inval) { return (short) (2 * inval); }

    public static long getLong() { return 7_000_000_000_000L; }
    public static long timesTwoLong(long inval) { return 2L * inval; }

    public static float getFloat() { return 42.0f; }
    public static float timesFiveFloat(float inval) { return inval * 5; }

    public static double getDouble() { return 42.0; }
    public static double timesFiveDouble(double inval) { return inval * 5; }

    public static byte getByte() { return (byte) 0xA1; }
    public static byte timesTwoByte(byte inval) { return (byte) (2 * inval); }

    public static char getChar() { return '!'; }
    public static char minusOneChar(char inval) { return (char) (inval - 1); }

    public static boolean getBoolean() { return true; }
    public static boolean negateBoolean(boolean inval) { return !inval; }

    public static String getString() { return "Welcome to the JVM"; }
    public static String timesTwoString(String inval) { return inval + inval; }
}
