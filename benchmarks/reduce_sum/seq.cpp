
#include "reduce_sum.hpp"
#include <algorithm>
#include <numeric>

// reduce_seq
int no_opt = 42;
void reduce_seq(unsigned nthreads) {
  double sum = 0.0;
  sum = std::reduce(vec.begin(), vec.end(), sum);
  no_opt += sum;
}

std::chrono::nanoseconds measure_time_seq(unsigned num_threads) {
  auto beg = std::chrono::high_resolution_clock::now();
  reduce_seq(num_threads);
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - beg);
}