#include "reduce_sum.hpp"
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>

// reduce_sum_hpx
void reduce_sum_hpx(unsigned nthreads) {
  double sum = 0.0;
  sum = hpx::reduce(hpx::execution::par, vec.begin(), vec.end(), sum);
}

std::chrono::nanoseconds measure_time_hpx(unsigned num_threads) {
  auto beg = std::chrono::high_resolution_clock::now();
  reduce_sum_hpx(num_threads);
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - beg);
}

void reduce_sum(
  const std::string& model,
  const unsigned num_threads,
  const unsigned num_rounds
  ) {

  std::cout << std::setw(12) << "size"
            << std::setw(12) << "runtime"
            << std::endl;


  for(size_t N=std::pow(2, 4); N<=std::pow(2, 25); N = N*2) {
    vec.resize(N);

    double runtime {0.0};

    for(unsigned j=0; j<num_rounds; ++j) {

      for(auto& d : vec) {
        d = ::rand();
      }

     if(model == "hpx") {
        runtime += measure_time_hpx(num_threads).count();
      }
      else assert(false);
    }

    std::cout << std::setw(12) << N
              << std::setw(12) << runtime / num_rounds / 1e9
              << std::endl;
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

  reduce_sum(model, num_threads, num_rounds);

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