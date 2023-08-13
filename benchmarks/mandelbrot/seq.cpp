#include "mandel.hpp"

void mandelbrot_seq(unsigned num_threads, int d = D) {
  int i,j ;
  for(i=0; i<H ;i ++) {
    for(j=0; j<W; j++) {

      auto xy = scale_xy(i, j);
      auto value = escape_time(xy.first, xy.second, d);
      auto k = 3*(j*W + i);
      std::tie(RGB[k], RGB[k+1], RGB[k+2]) = get_color(value);
    }
  }
}

std::chrono::nanoseconds measure_time_seq(unsigned num_threads) {
  auto beg = std::chrono::high_resolution_clock::now();
  mandelbrot_seq(num_threads);
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - beg);
}
