
#include "scan.hpp"
#include <algorithm>
#include <numeric>

// scan_seq
void scan_seq(std::size_t nthreads) {
  std::inclusive_scan(
    input.begin(), input.end(), output.begin(), std::multiplies<int>{}
  );
}

std::chrono::nanoseconds measure_time_seq(std::size_t num_threads) {
  auto beg = std::chrono::high_resolution_clock::now();
  scan_seq(num_threads);
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - beg);
}