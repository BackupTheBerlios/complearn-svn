public class CompLearnJ {
  public native double ncd(String a, String b);
  static { System.loadLibrary("CompLearnJ"); }
  public static void main(String[] args) {
    double result;
    result = new CompLearnJ().ncd("hello", "world");
    System.out.println("The NCD is " + result);
  }
}

