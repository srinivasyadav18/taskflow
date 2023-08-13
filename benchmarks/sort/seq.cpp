#include "sort.hpp"
#include <algorithm>

void sort_seq(size_t num_threads) {
  std::sort(vec.begin(), vec.end());
}

std::chrono::nanoseconds measure_time_seq(size_t num_threads) {
  auto beg = std::chrono::high_resolution_clock::now();
  sort_seq(num_threads);
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - beg);
}


