public class MainJava {
    public static void main(String[] args) {
        MainJavaTest test;
        test = new MainJavaTest();
        test.size = MainJavaTest.FreshJuiceSize.MEDIUM;
        System.out.println(test.size);
    }
}
