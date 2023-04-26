//
// dev.tmich.jnipp.test.TestStructEquiv.java
// jni++
//
// Created by Thomas Micheline Apr 19, 2023.
//
// Copyright © 2023 Thomas Micheline All rights reserved.
//
// This code is licensed under the 2-clause BSD license (see LICENSE.md for details)
//

package dev.tmich.jnipp.test;

import dev.tmich.jnipp.test.swig.TestStruct;

public class TestStructEquiv {

  public TestStructEquiv() {
  }

  public TestStructEquiv(TestStruct ts) {
    l = ts.getL();
    i = ts.getI();
    c = ts.getC();
  }

  public void setC(char value) {c = value;}
  public char getC() {return c;}

  public void setI(int value) {i = value;}
  public int getI() {return i;}

  public void setL(long value) {l = value;}
  public long getL() {return l;}

  public static TestStructEquiv timesFive(TestStructEquiv orig) {
    TestStructEquiv tse = new TestStructEquiv();
    tse.c = (char) (orig.getC() * 5);
    tse.i = orig.getI() * 5;
    tse.l = orig.getL() * 5;
    return tse;
  }

  private long l;
  private int i;
  private char c;
}
