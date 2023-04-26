//
// dev.tmich.jnipp.test.TestInstancePrimitives.java
// jni++
//
// Created by Thomas Micheline Apr 19, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

package dev.tmich.jnipp.test;

public class TestInstancePrimitives {
    public int getInt() {return i;}
    public void setInt(int i) {this.i = i;}
    public long getLong() {return l;}
    public void setLong(long l) {this.l = l;}
    public short getShort() {return s;}
    public void setShort(short s) {this.s = s;}
    public byte getByte() {return b;}
    public void setByte(byte b) {this.b = b;}
    public char getChar() {return c;}
    public void setChar(char c) {this.c = c;}
    public float getFloat() {return f;}
    public void setFloat(float f) {this.f = f;}
    public double getDouble() {return d;}
    public void setDouble(double d) {this.d = d;}
    public boolean isBool() {return bool;}
    public void setBool(boolean bool) {this.bool = bool;}
    public String getStr() {return str;}
    public void setStr(String str) {this.str = str;}

    public int timesInt(int inval) { return i * inval; }
    public short timesShort(short inval) { return (short) (s * inval); }
    public long timesLong(long inval) { return l * inval; }
    public byte timesByte(byte inval) { return (byte) (b * inval); }
    public char minusChar(char inval) { return (char) (c - inval); }

    public float timesFloat(float inval) { return inval * f; }
    public double timesDouble(double inval) { return inval * d; }

    public boolean negateBoolean() { return !bool; }
    public String addString(String inval) { return str + inval; }

    private int i = 7_000_000;
    private long l = 7_000_000_000_000L;
    private short s = 7_000;
    private byte b = 70;
    private char c = '7';

    private float f = 7.0F;
    private double d = 7.777777777;

    private boolean bool = false;
    private String str = "Welcome to the JVM";
}
