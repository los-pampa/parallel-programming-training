#include <iostream>
#include <sstream>
#include <omp.h>

using namespace std;

void sum(char* output, const long unsigned int d, const long unsigned int n) { // 1000 10000000
	long unsigned int digits[d + 11]; // Array[1011]
	
	for (long unsigned int digit = 0; digit < d + 11; ++digit) {
		digits[digit] = 0;
	}

	#pragma omp parallel
	{
		long unsigned int digits_local[d + 11]; // Array[1011]
		for (long unsigned int digit = 0; digit < d + 11; ++digit) {
			digits_local[digit] = 0;
		}

		#pragma omp for
		for (long unsigned int i = 1; i <= n; ++i) {
			long unsigned int remainder = 1;
			for (long unsigned int digit = 0; digit < d + 11 && remainder; ++digit) {
				
				digits_local[digit] += (remainder / i);
				remainder = (remainder % i) * 10;
			}
		}

		#pragma omp critical
		{
			for (long unsigned int digit = 0; digit < d + 11; ++digit) {
				digits[digit] += digits_local[digit];
			}
		}	
	}

	
	
	for (long unsigned int i = d + 11 - 1; i > 0; --i) { 
		digits[i - 1] += digits[i] / 10;
		digits[i] %= 10;
	}
	if (digits[d + 1] >= 5) {
		++digits[d];
	}
	for (long unsigned int i = d; i > 0; --i) {
		digits[i - 1] += digits[i] / 10;
		digits[i] %= 10;
	}
	stringstream stringstreamA;
	stringstreamA << digits[0] << ".";
	for (long unsigned int i = 1; i <= d; ++i) {
		stringstreamA << digits[i];
	}
	stringstreamA << '\0';
	string stringA = stringstreamA.str();
	stringA.copy(output, stringA.size());
}

int main() {

	long unsigned int d, n;

	cin >> d >> n;//1000 10000000

	char output[d + 10]; // Array (1010)

	sum(output, d, n);

	cout << output << endl;

	return 0;
}
