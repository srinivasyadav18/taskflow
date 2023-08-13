#include "for_each.hpp"

void for_each_seq(size_t nthreads) {
  for(size_t i=0; i<vec.size(); i++) {
    vec[i] = std::tan(vec[i]);
  }
}

std::chrono::nanoseconds measure_time_seq(size_t num_threads) {
  auto beg = std::chrono::high_resolution_clock::now();
  for_each_seq(num_threads);
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - beg);
}

