
public class KMPMatcher {
	public void match(char[] text, char[] pattern) {
		int n = text.length - 1;
		int m = pattern.length - 1;
		int[] pi = this.computePrefixFunction(pattern);
		int q = 0;
		
		for (int i = 1; i <= n; i++) {
			while (q > 0 && pattern[q + 1] != text[i]) {
				q = pi[q];
			}
			
			if (pattern[q + 1] == text[i]) {
				q = q + 1;
			}
			
			if (q == m) {
				System.out.println("Wzorzec występuje z przesunięciem " + (i - m));
				q = pi[q];
			}
		}
	}
	
	public int[] computePrefixFunction(char[] pattern) {
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
		KMPMatcher kmpm = new KMPMatcher();
		
		String text = "abababababababa";
		String pattern = "abab";
		
		kmpm.match((" "+text).toCharArray(), (" "+pattern).toCharArray());
	}
}
