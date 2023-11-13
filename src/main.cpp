#include <cstdio>
#include <sstream>
#include <vector>

class Benchmark {};

class CsvRow {
   private:
    const int M;
    const std::string openmp_scheduling_method;
    const int chunk_sz;
    const double t1;
    const double t2;
    const double t4;
    const double t8;
    const double s2;
    const double s4;
    const double s8;

   public:
    CsvRow(int M, std::string openmp_scheduling_method, int chunk_sz, double t1,
           double t2, double t3, double t4, double t8)
        : M(M),
          openmp_scheduling_method(openmp_scheduling_method),
          chunk_sz(chunk_sz),
          t1(t1),
          t2(t2),
          t4(t4),
          t8(t8),
          s2(t2 / t1),
          s4(t4 / t1),
          s8(t8 / t1) {}

    std::string to_string() const {
        std::stringstream ss;
        ss << M << ", " << openmp_scheduling_method << ", " << chunk_sz << ", "
           << t1 << ", " << t2 << ", " << t4 << ", " << t8 << ", " << s2 << ", "
           << s4 << ", " << s8;
        return ss.str();
    }
};

void generate_csv_row() {}

void generate_primes(const int N) {
    std::vector<int> prime = {2};

    // we add 2 because all priems are odd (except for 2)
    // so we can step += 2 instead of +=1
    for (int n = 3; n < N; n += 2) {
        bool isPrime = true;

        for (int k = 0; prime[k] * prime[k] <= n; ++k) {
            // check if number n can be broken down using another prime
            // ie can be prime factorized
            if (n % prime[k] == 0) {
                isPrime = false;
                break;
            }
        }

        if (isPrime) prime.push_back(n);
    }

    for (int c : prime) printf("%d\n", c);
}

int main() {
    generate_primes(50);
    return 0;
}
