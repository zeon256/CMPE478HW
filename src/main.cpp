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
    
    /**
     * @brief Find primes from 3 to N using OpenMP scheduling
     * Method is templated to allow for static, dynamic, and guided scheduling
     * so that we don't have to write 3 separate functions
     * @tparam schedule_type OpenMP scheduling type
     */
    template <const omp_sched_t schedule_type>
    static void find_primes(const int N, const int T, const int C) {
        omp_set_num_threads(T);
        omp_set_schedule(schedule_type, C);
        std::vector<int> primes{2};

        // clang-format off
        #pragma omp parallel for schedule(runtime) private(primes)
        for (int n = 3; n < N; n += 2) {
            bool isPrime = true;

            for(int prime : primes) {
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
    requires std::invocable<F&, const int, const int, const int>
    inline double time_benchmark(const int m, const int T, const int C,
                                 F f) {
        auto begin = std::chrono::steady_clock::now();

        f(m, T, C);

        auto end = std::chrono::steady_clock::now();

        auto time_taken =
            std::chrono::duration_cast<std::chrono::microseconds>(end - begin);

        return (double)time_taken.count();
    }

    /**
     * @brief Convert matrix to csv and write to file
     * @param matrix Matrix to convert
    */
    void matrix_to_csv(std::vector<std::array<std::string, 10>> const& matrix) {
        printf("[+] Writing to csv\n");
        std::stringstream ss;

        for (auto row : matrix) {
            for (auto col : row) {
                ss << col << ",";
            }
            ss << "\n";
        }

        std::ofstream outfile("benchmark.csv");
        outfile << ss.str();
        outfile.close();
    }

    /**
     * @brief Run benchmark and insert to csv row
     * @tparam F Function type
     * @param scheduler_name Name of scheduler
     * @param chunk Chunk size
     * @param m Number of primes to find
     * @param csv CSV matrix
     * @param f Function to benchmark
    */
    template <typename F>
    requires std::invocable<F&, const int, const int, const int>
    void run_and_insert_to_csv_row(std::string scheduler_name, 
        const int chunk,
        const int m,
        std::vector<std::array<std::string, 10>>& csv,
        F f) {
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
        // csv matrix, we only have 10 columns so we can use std::array
        std::vector<std::array<std::string, 10>> csv;

        // header
        csv.push_back({"M", "OpenMP Loop Scheduling Method", "Chunk Size", "T1",
                       "T2", "T4", "T8", "S2", "S4", "S8"});

        std::vector<int> chunk_size = {2, 16, 32, 64, 128, 256};
        
        // start at 1000
        for (int m = 1000; m <= max_m; m += step) {
            printf("[M = %d] Starting benchmark\n", m);

            for (int chunk : chunk_size) {
                run_and_insert_to_csv_row("static", chunk, m, csv,
                                        find_primes<omp_sched_static>);
                run_and_insert_to_csv_row("dynamic", chunk, m, csv,
                                          find_primes<omp_sched_dynamic>);
                run_and_insert_to_csv_row("guided", chunk, m, csv,
                                          find_primes<omp_sched_guided>);
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
