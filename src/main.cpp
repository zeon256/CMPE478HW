#include <chrono>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <vector>

#include "omp.h"

class Benchmark {
   private:
    const int max_m;

    static inline std::vector<int> find_primes_sqrtn(const int sqrt_n) {
        std::vector<int> primes{2};

        // find primes from 3 to sqrt(N)
        // inherently sequential
        for (int n = 3; n <= sqrt_n; n += 2) {
            bool is_prime = true;

            for (int prime : primes) {
                if (prime * prime > n) break;

                // if number can be prime factorized => not prime
                if (n % prime == 0) {
                    is_prime = false;
                    break;
                }
            }

            if (is_prime) primes.push_back(n);
        }

        return primes;
    }

    /**
     * @brief Find primes from 3 to N using OpenMP scheduling
     * Method is templated to allow for static, dynamic, and guided scheduling
     * so that we don't have to write 3 separate functions
     * @tparam schedule_type OpenMP scheduling type
     */
    template <const omp_sched_t schedule_type>
    static auto find_primes(const int M, const int T, const int C)
        -> std::pair<std::vector<int>, std::vector<int>> {
        omp_set_num_threads(T);
        omp_set_schedule(schedule_type, C);

        const int sqrt_n = (int)sqrt(M);

        std::vector<int> primes = find_primes_sqrtn(sqrt_n);
        std::vector<int> par_result;
        std::vector<int> local_primes;

        // clang-format off
        #pragma omp parallel private(local_primes) shared(primes, par_result)
        {
            // find primes from sqrt(N) + 1 to N
            // parallelize this part
            // we can do this because we know that the primes from 3 to sqrt(N)
            // we want to skip even numbers, so we start at sqrt(N) + 1 if sqrt(N) is even
            // and sqrt(N) + 2 if sqrt(N) is odd
            #pragma omp for schedule(runtime) nowait
            for (int n = ((sqrt_n + 1) & 1) ? sqrt_n + 1 : sqrt_n + 2; n <= M; n+=2) {
                bool is_prime = true;

                for(int prime : primes) {
                    // if number can be prime factorized => not prime
                    if (n % prime == 0) {
                        is_prime = false;
                        break;
                    }
                }

                if (is_prime) local_primes.push_back(n);
            }

            #pragma omp critical
            par_result.insert(par_result.end(), local_primes.begin(), local_primes.end());
        }

        // we dont do printing here because we dont want to time it

        return { primes, par_result };
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
    inline auto time_benchmark(const int m, const int T, const int C,
                                 F f) -> double {
        auto begin = std::chrono::steady_clock::now();

        auto [primes, result] = f(m, T, C);        

        auto end = std::chrono::steady_clock::now();
        auto time_taken =
            std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
        
        std::sort(result.begin(), result.end());

        for(int prime : primes) printf("%d\n", prime);
        for(int prime : result) printf("%d\n", prime);

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
    void run_and_insert_csv(
        std::string scheduler_name, 
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
            std::to_string(m),
            scheduler_name,
            std::to_string(chunk)
        };

        for (int i = 0; i < timings.size(); ++i) {
            timings_str[i + 3] = std::to_string(timings[i]);
        }

        // insert to csv
        csv.push_back(std::array<std::string, 10>(timings_str));
    }

   public:
    Benchmark(int max_m) : max_m(max_m) {}

    void run_benchmark() {
        // csv matrix, we only have 10 columns so we can use std::array
        std::vector<std::array<std::string, 10>> csv;

        // header
        csv.push_back({"M", "OpenMP Loop Scheduling Method", "Chunk Size", "T1",
                       "T2", "T4", "T8", "S2", "S4", "S8"});
        
        // start at 1000
        for (int M = 1000; M <= max_m; M *= 10) {
            printf("[M = %d] Starting benchmark\n", M);
            
            // 16 32 64 128 256
            for (int chunk = 16; chunk <= 256; chunk *= 2) {
                run_and_insert_csv("static", chunk, M, csv, find_primes<omp_sched_static>);
                run_and_insert_csv("dynamic", chunk, M, csv, find_primes<omp_sched_dynamic>);
                run_and_insert_csv("guided", chunk, M, csv, find_primes<omp_sched_guided>);
            }
        }

        matrix_to_csv(csv);
    }
};

auto main() -> int {
    auto bench = Benchmark(1e6);
    bench.run_benchmark();

    return 0;
}
