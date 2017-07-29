import java.lang.*;

public class JNIDigitRecog {
	public JNIDigitRecog() {
		System.loadLibrary("libjni_wrapper");
	}

	native int predict(String filename);
	
	public static void main(String[] args) {
		if(args[0] == null) {
			System.out.println("specify the filename of picture");
			System.exit(0);
		}
		try {
			JNIDigitRecog dr = new JNIDigitRecog();
			int num = dr.predict(args[0]);
			System.out.println("Predicted number = " + num);
		} catch (UnsatisfiedLinkError e) {
			System.err.println("Native code library failed to load.\n" + e);
			System.exit(1);
		} catch(RuntimeException e) {
			System.err.println("RuntimeException" + e);
			System.exit(1);
		}
	}
}
