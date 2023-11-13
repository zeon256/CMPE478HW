#include <chrono>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <vector>

class Benchmark {
   private:
    const int max_m;
    const int step;

    static void find_primes_static(const int N, const int T) {
        std::vector<int> primes{2};

        for (int n = 3; primes.size() < N; n += 2) {
            bool isPrime = true;

            for (int i = 0; i < primes.size(); ++i) {
                int prime = primes[i];
                if (prime * prime > n) break;

                // if number can be prime factorized => not prime
                if (n % prime == 0) {
                    isPrime = false;
                    break;
                }
            }

            if (isPrime) primes.push_back(n);
        }
    }

    /**
     * @brief Benchmark a function which takes in m
     *
     * @tparam F Function type
     * @param m Number of primes to find
     * @param f Function to benchmark
     */
    template <typename F>
    inline long long time_benchmark(const int m, const int T, F f) {
        std::chrono::steady_clock::time_point begin =
            std::chrono::steady_clock::now();

        f(m, T);

        std::chrono::steady_clock::time_point end =
            std::chrono::steady_clock::now();

        auto time_taken =
            std::chrono::duration_cast<std::chrono::microseconds>(end - begin);

        return time_taken.count();
    }

   public:
    Benchmark(int max_m, int step) : max_m(max_m), step(step) {}

    void run_benchmark() {
        std::vector<std::array<std::string, 10>> csv;

        // header
        csv.push_back({"M", "OpenMP Loop Scheduling Method", "Chunk Size", "T1",
                       "T2", "T4", "T8", "S2", "S4", "S8"});

        for (int m = step; m <= max_m; m += step) {
            printf("[M = %d] Starting benchmark\n", m);

            std::vector<long long> timings = {
                time_benchmark(m, 1, find_primes_static),
                time_benchmark(m, 2, find_primes_static),
                time_benchmark(m, 4, find_primes_static),
                time_benchmark(m, 8, find_primes_static),
            };

            // append speed up to timings
            const int T_SZ = timings.size();
            for (int i = 1; i < T_SZ; ++i) {
                timings.push_back((double)timings[0] / timings[i]);
            }

            // insert to csv
            csv.push_back({std::to_string(m), "static", "1",
                           std::to_string(timings[0]),
                           std::to_string(timings[1]),
                           std::to_string(timings[2]),
                           std::to_string(timings[3]),
                           std::to_string(timings[4]),
                           std::to_string(timings[5]),
                           std::to_string(timings[6])});            
        }

        // convert csv matrix to string
        std::stringstream ss;
        for (auto row : csv) {
            for (auto col : row) {
                ss << col << ",";
            }
            ss << "\n";
        }

        // create file and write to it
        std::ofstream outfile("benchmark.csv");
        outfile << ss.str();
        outfile.close();
    }
};

int main() {
    auto bench = Benchmark(10000, 1000);
    bench.run_benchmark();

    return 0;
}
