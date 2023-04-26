//
// dev.tmich.jnipp.test.TestBoxed.java
// jni++
//
// Created by Thomas Micheline Apr 19, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

package dev.tmich.jnipp.test;

public class TestBoxed {
    public Integer getInt() {return i;}
    public void setInt(Integer i) {this.i = i;}
    public Long getLong() {return l;}
    public void setLong(Long l) {this.l = l;}
    public Short getShort() {return s;}
    public void setShort(Short s) {this.s = s;}
    public Byte getByte() {return b;}
    public void setByte(Byte b) {this.b = b;}
    public Character getCharacter() {return c;}
    public void setCharacter(Character c) {this.c = c;}
    public Float getFloat() {return f;}
    public void setFloat(Float f) {this.f = f;}
    public Double getDouble() {return d;}
    public void setDouble(Double d) {this.d = d;}
    public Boolean getBoolean() {return z;}
    public void setBoolean(Boolean z) {this.z = z;}

    public Integer timesInt(Integer inval) { return i * inval; }
    public Short timesShort(Short inval) { return (short) (s * inval); }
    public Long timesLong(Long inval) { return l * inval; }
    public Byte timesByte(Byte inval) { return (byte) (b * inval); }
    public Character minusCharacter(Character inval) { return (char) (c - inval); }

    public Float timesFloat(Float inval) { return inval * f; }
    public Double timesDouble(Double inval) { return inval * d; }

    public Boolean xorBool(Boolean b) { return b ^ z; }

    // If you change these default values, you will likely need to update test cases as well
    private Integer i = 7_000_000;
    private Long l = 7_000_000_000_000L;
    private Short s = 7_000;
    private Byte b = 70;
    private Character c = '7';

    private Float f = 7.0F;
    private Double d = 7.777777777;

    private Boolean z = true;
}
