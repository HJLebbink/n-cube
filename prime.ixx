module;
#include <string>
#include <sstream>
#include <vector>

export module prime;

namespace prime {

	namespace {
		constexpr int MAXN = 10000;
		int spf[MAXN];
		bool spf_filled = false;

		void sieve()
		{
			spf[1] = 1;
			for (int i = 2; i < MAXN; i++)

				// marking smallest prime factor for every 
				// number to be itself. 
				spf[i] = i;

			// separately marking spf for every even 
			// number as 2 
			for (int i = 4; i < MAXN; i += 2) {
				spf[i] = 2;
			}
			for (int i = 3; (i * i) < MAXN; i++)
			{
				// checking if i is prime 
				if (spf[i] == i)
				{
					// marking SPF for all numbers divisible by i 
					for (int j = i * i; j < MAXN; j += i) {
						// marking spf[j] if it is not  
						// previously marked 
						if (spf[j] == j) {
							spf[j] = i;
						}
					}
				}
			}
		}

	}
	
	// A O(log n) function returning primefactorization 
	// by dividing by smallest prime factor at every step 
	export inline [[nodiscard]]
	std::vector<int> get_factorization(int x)
	{
		if (!spf_filled) {
			sieve();
			spf_filled = true;
		}
		std::vector<int> ret;
		while (x != 1)
		{
			ret.push_back(spf[x]);
			x = x / spf[x];
		}
		return ret;
	}

	export inline [[nodiscard]]
	std::string vector_to_string(const std::vector<int>& a) {
		std::stringstream result;
		const int size = static_cast<int>(a.size());

		for (const int p : {2, 3, 5, 7, 11, 13}) {
			int count = 0;
			for (int i = 0; i < size; ++i) {
				if (a[i] == p) {
					count++;
				}
			}
			if (count > 0) {
				result << p << "^" << count << " + ";
			}
		}
		return result.str().substr(0, result.str().length() - 3);
	}
}