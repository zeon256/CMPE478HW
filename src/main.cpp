#include <stdio.h>

const int N = 50;
int prime[N];

int main() {    
    int count = 1; // Number of primes found
    prime[0] = 2;
    
    // we add 2 because all priems are odd (except for 2)
    // so we can step += 2 instead of +=1
    for (int n = 3; count < N; n += 2) {
        bool isPrime = true;

        for (int k = 0; k < count && prime[k] * prime[k] <= n; ++k) {
            // check if number n can be broken down using another prime
            // ie can be prime factorized
            if (n % prime[k] == 0) {
                isPrime = false;
                break;
            }
        }

        if (isPrime) prime[count++] = n;
    }

    for(int c : prime) printf("%d\n", c);

    return 0;
}
