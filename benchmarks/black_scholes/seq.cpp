#include "common.hpp"

void bs_seq_parallel_for(unsigned num_threads) {
  int i, j;
  float price;

  for (j=0; j<NUM_RUNS; j++) {
    for (i=0; i<numOptions; i++) {
      /* Calling main function to calculate option value based on
       * Black & Scholes's equation.
       */
      price = BlkSchlsEqEuroNoDiv( sptprice[i], strike[i],
          rate[i], volatility[i], otime[i],
          otype[i], 0);
      prices[i] = price;

#ifdef ERR_CHK
      check_error(i, price);
#endif
    }
  }
}

std::chrono::nanoseconds measure_time_seq(unsigned num_threads) {
  auto beg = std::chrono::high_resolution_clock::now();
  bs_seq_parallel_for(num_threads);
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - beg);
}


