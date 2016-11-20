
public class KMPMatcher {
	public void match(char[] text, char[] pattern) {
		int n = text.length - 1;
		int m = pattern.length - 1;
		int[] pi = this.computePrefixArray(pattern);
		int q = 0;
		
		for (int i = 1; i <= n; i++) {
			while (q > 0 && pattern[q + 1] != text[i]) {
				q = pi[q];
			}
			
			if (pattern[q + 1] == text[i]) {
				q = q + 1;
			}
			
			if (q == m) {
				System.out.println("\tWzorzec występuje z przesunięciem " + (i - m));
				q = pi[q];
			}
		}
	}
	
	public int[] computePrefixArray(char[] pattern) {
		int m = pattern.length - 1;
		int[] pi = new int[m + 1];
		int k = 0;
		
		pi[1] = 0;
		
		for (int q = 2; q <= m; q++) {
			while (k > 0 && pattern[k + 1] != pattern[q]) {
				k = pi[k];
			}
			
			if (pattern[k + 1] == pattern[q]) {
				k = k + 1;
			}
			
			pi[q] = k;
		}
		
		return pi;
	}
	
	public static void main(String[] args) {
		if (args.length < 2) {
			System.out.println("\n\tUżycie: <TEKST> <WZORZEC>\n");
			System.exit(-1);
		}
		
		KMPMatcher kmpm = new KMPMatcher();
		
		String text = args[0];
		String pattern = args[1];
		
		if (pattern.length() == 0) {
			System.out.println("\n\tWzorzec nie może być pusty\n");
			System.exit(-1);
		}
		
		kmpm.match((" "+text).toCharArray(), (" "+pattern).toCharArray());
	}
}
