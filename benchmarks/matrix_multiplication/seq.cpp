#include "matrix_multiplication.hpp"

// matrix_multiplication_omp
// reference: https://computing.llnl.gov/tutorials/openMP/samples/C/omp_mm.c
void matrix_multiplication_seq(unsigned nthreads) {

  int i, j, k;

  for(i=0; i<N; ++i) {
    for(j=0; j<N; j++) {
      a[i][j] = i + j;
    }
  }

  for(i=0; i<N; ++i) {
    for(j=0; j<N; j++) {
      b[i][j] = i * j;
    }
  }

  for(i=0; i<N; ++i) {
    for(j=0; j<N; j++) {
      c[i][j] = 0;
    }
  }

  for(i=0; i<N; ++i) {
    for(j=0; j<N; j++) {
      for (k=0; k<N; k++) {
        c[i][j] += a[i][k] * b[k][j];
      }
    }
  }
}

std::chrono::nanoseconds measure_time_seq(unsigned num_threads) {
  auto beg = std::chrono::high_resolution_clock::now();
  matrix_multiplication_seq(num_threads);
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - beg);
}
