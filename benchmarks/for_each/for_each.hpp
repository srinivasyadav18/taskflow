#pragma once

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

std::chrono::nanoseconds measure_time_taskflow(size_t);
std::chrono::nanoseconds measure_time_tbb(size_t);
std::chrono::nanoseconds measure_time_omp(size_t);
std::chrono::nanoseconds measure_time_seq(size_t);
