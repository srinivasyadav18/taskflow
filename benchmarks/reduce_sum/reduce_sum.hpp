#include <algorithm>
#include <cassert>
#include <cstdio>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <thread>
#include <random>
#include <cmath>
#include <atomic>

inline std::vector<double> vec;

std::chrono::nanoseconds measure_time_taskflow(unsigned);
std::chrono::nanoseconds measure_time_tbb(unsigned);
std::chrono::nanoseconds measure_time_omp(unsigned);
std::chrono::nanoseconds measure_time_seq(unsigned);
