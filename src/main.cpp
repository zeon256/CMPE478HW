#include <chrono>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <vector>

#include "omp.h"

class Benchmark {
   private:
    const int max_m;
    const int step;

    static void find_primes_static(const int N, const int T, const int C) {
        omp_set_num_threads(T);
        std::vector<int> primes{2};

#pragma omp parallel for schedule(static, C) private(primes)
        for (int n = 3; n < N; n += 2) {
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

    static void find_primes_dynamic(const int N, const int T, const int C) {
        omp_set_num_threads(T);
        std::vector<int> primes{2};

#pragma omp parallel for schedule(dynamic, C) private(primes)
        for (int n = 3; n < N; n += 2) {
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

    static void find_primes_guided(const int N, const int T, const int C) {
        omp_set_num_threads(T);
        std::vector<int> primes{2};

#pragma omp parallel for schedule(guided, C) private(primes)
        for (int n = 3; n < N; n += 2) {
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
     * @param T Number of threads
     * @param C Chunk size
     * @param f Function to benchmark
     */
    template <typename F>
    inline double time_benchmark(const int m, const int T, const int C,
                                 const F f) {
        std::chrono::steady_clock::time_point begin =
            std::chrono::steady_clock::now();

        f(m, T, C);

        std::chrono::steady_clock::time_point end =
            std::chrono::steady_clock::now();

        auto time_taken =
            std::chrono::duration_cast<std::chrono::microseconds>(end - begin);

        return (double)time_taken.count();
    }

    void matrix_to_csv(std::vector<std::array<std::string, 10>> const& matrix) {
        std::stringstream ss;

        for (auto row : matrix) {
            for (auto col : row) {
                ss << col << ",";
            }
            ss << "\n";
        }

        std::ofstream outfile("../benchmark.csv");
        outfile << ss.str();
        outfile.close();
    }

    template <typename F>
    void run_and_insert_to_csv_row(
        std::string scheduler_name, const int chunk, const int m,
        std::vector<std::array<std::string, 10>>& csv, F f) {
        std::vector<double> timings;

        // num threads
        for (int t = 1; t <= 8; t *= 2) {
            timings.push_back(time_benchmark(m, t, chunk, f));
        }

        // append speed up to timings
        const int T_SZ = timings.size();
        for (int i = 1; i < T_SZ; ++i) {
            timings.push_back((double)timings[0] / timings[i]);
        }

        std::array<std::string, 10> timings_str = {
            std::to_string(m), scheduler_name, std::to_string(chunk)};

        for (int i = 0; i < timings.size(); ++i) {
            timings_str[i + 3] = std::to_string(timings[i]);
        }

        // insert to csv
        csv.push_back(std::array<std::string, 10>(timings_str));
    }

   public:
    Benchmark(int max_m, int step) : max_m(max_m), step(step) {}

    void run_benchmark() {
        std::vector<std::array<std::string, 10>> csv;

        // header
        csv.push_back({"M", "OpenMP Loop Scheduling Method", "Chunk Size", "T1",
                       "T2", "T4", "T8", "S2", "S4", "S8"});

        std::vector<int> chunk_size = {2, 16, 32, 64, 128, 256};

        for (int m = 0; m <= max_m; m += step) {
            printf("[M = %d] Starting benchmark\n", m);

            for (int chunk : chunk_size) {
                run_and_insert_to_csv_row("static", chunk, m, csv,
                                          find_primes_static);
                run_and_insert_to_csv_row("dynamic", chunk, m, csv,
                                          find_primes_dynamic);
                run_and_insert_to_csv_row("guided", chunk, m, csv,
                                          find_primes_guided);
            }
        }

        matrix_to_csv(csv);
    }
};

int main() {
    auto bench = Benchmark(100000, 1000);
    bench.run_benchmark();

    return 0;
}
