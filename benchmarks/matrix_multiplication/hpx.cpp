#include "matrix_multiplication.hpp"
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>

// matrix_multiplication_hpx
void matrix_multiplication_hpx(unsigned num_threads) {

  auto init_a = hpx::experimental::for_loop(hpx::execution::par(hpx::execution::task), (int) 0, N, [&] (int i) {
  //  hpx::experimental::for_loop(hpx::execution::par, 0, N, [&] (auto i) {
    for(int j=0; j<N; ++j) {
      a[i][j] = i + j;
    }
  });

  auto init_b = hpx::experimental::for_loop(hpx::execution::par(hpx::execution::task), (int) 0, N, [&] (int i) {
  //  hpx::experimental::for_loop(hpx::execution::par, 0, N, [&] (auto i) {
    for(int j=0; j<N; ++j) {
      b[i][j] = i * j;
    }
  });

  auto init_c = hpx::experimental::for_loop(hpx::execution::par(hpx::execution::task), (int) 0, N, [&] (int i) {
  //  hpx::experimental::for_loop(hpx::execution::par, 0, N, [&] (auto i) {
    for(int j=0; j<N; ++j) {
      c[i][j] = 0;
    }
  });

  hpx::wait_all(init_a, init_b, init_c);
  hpx::experimental::for_loop(hpx::execution::par, 0, N, [&] (auto i) {
    for(int j=0; j<N; ++j) {
      for(int k=0; k<N; k++) {
        c[i][j] += a[i][k] * b[k][j];
      }
    }
  });
}

std::chrono::nanoseconds measure_time_hpx(unsigned num_threads) {
  auto beg = std::chrono::high_resolution_clock::now();
  matrix_multiplication_hpx(num_threads);
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - beg);
}

int N = 0;
double **a = nullptr, **b = nullptr, **c = nullptr;

void matrix_multiplication(
  const std::string& model,
  const unsigned num_threads,
  const unsigned num_rounds
  ) {

  std::cout << std::setw(12) << "size"
            << std::setw(12) << "runtime"
            << std::endl;

  for(int i=128; i<=1024; i += 32) {

    N = i;

    allocate_matrix();

    double runtime {0.0};

    for(unsigned j=0; j<num_rounds; ++j) {
     if(model == "hpx") {
        runtime += measure_time_hpx(num_threads).count();
      }
      else assert(false);
    }

    std::cout << std::setw(12) << N
              << std::setw(12) << runtime / num_rounds / 1e9
              << std::endl;

    deallocate_matrix();
  }
}

int hpx_main(hpx::program_options::variables_map& vm) {

  std::string model = vm["m"].as<std::string>();
  std::size_t num_threads = vm["t"].as<std::size_t>();
  std::size_t num_rounds = vm["r"].as<std::size_t>();

  std::cout << "model=" <<  model  << ' '
            << "num_threads=" << num_threads  << ' '
            << "num_rounds=" << num_rounds << ' '
            << std::endl;
  
  matrix_multiplication(model, num_threads, num_rounds);

  return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace hpx::program_options;
    options_description cmdline("usage: " HPX_APPLICATION_STRING " [options]");
    // clang-format off
    cmdline.add_options()
        ("m",
        hpx::program_options::value<std::string>()->default_value("hpx"),
        "model = hpx")
        ("t",
        hpx::program_options::value<std::size_t>()->default_value(1),
        "number of threads")
        ("r",
        hpx::program_options::value<std::size_t>()->default_value(1),
        "Number of rounds");
    // clang-format on
    hpx::init_params init_args;
    init_args.desc_cmdline = cmdline;

    return hpx::init(hpx_main, argc, argv, init_args);
}