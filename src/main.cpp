#include <cstdio>
#include <vector>

class CsvRow {
private:
    int M;
    std::string openmp_scheduling_method;
    int chunk_sz;
    double t1;
    double t2;
    double t4;
    double t8;
    double s2;
    double s4;
    double s8;
};

auto generate_csv_row() -> void {}

auto generate_primes(const int N) -> std::vector<int> {
    std::vector<int> prime(N, 2);
    
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
    
    return prime;
}


auto main() -> int {

    for(int c : generate_primes(50)) printf("%d\n", c);

    return 0;
}
