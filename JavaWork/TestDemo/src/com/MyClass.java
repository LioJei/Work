package com;

class MyClass {
    private final String myName;
   public MyClass(String name) {
       myName = name;
       System.out.println(myName + "constructor called");
   }
    public void MyPrint(String string) {
       System.out.println(myName + " prints: " + string);
   }
}
