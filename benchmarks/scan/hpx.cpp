#include "scan.hpp"
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>

// scan_hpx
void scan_hpx(unsigned nthreads) {

  hpx::inclusive_scan(hpx::execution::par,
    input.begin(), input.end(), output.begin(), std::multiplies<int>{}
  );
}

std::chrono::nanoseconds measure_time_hpx(unsigned num_threads) {
  auto beg = std::chrono::high_resolution_clock::now();
  scan_hpx(num_threads);
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - beg);
}

void scan(
  const std::string& model,
  const unsigned num_threads,
  const unsigned num_rounds
  ) {

  std::cout << std::setw(12) << "size"
            << std::setw(12) << "runtime"
            << std::endl;

  for(size_t N=std::pow(2, 4); N<=std::pow(2, 25); N = N*2) {
    input.resize(N);
    output.resize(N);

    double runtime {0.0};

    for(unsigned j=0; j<num_rounds; ++j) {

      for(auto& d : input) {
        d = ::rand();
      }

     if(model == "hpx") {
        runtime += measure_time_hpx(num_threads).count();
      } else if (model == "hpx_simd"){
        runtime += measure_time_hpx(num_threads).count();
      }
      else assert(false);
    }

    std::cout << std::setw(12) << N
              << std::setw(12) << runtime / num_rounds / double(1e9)
              << std::endl;
  }
}

int hpx_main(hpx::program_options::variables_map& vm) {

  std::string model = vm["m"].as<std::string>();
  std::size_t num_threads = vm["t"].as<std::size_t>();
  std::size_t num_rounds = vm["r"].as<std::size_t>();

  if (num_threads != hpx::get_num_worker_threads())
  {
    std::cout << "ERROR!!" << std::endl;
    return hpx::finalize();
  }
  std::cout << "model=" <<  model  << ' '
            << "num_threads=" << hpx::get_num_worker_threads() << ' '
            << "num_rounds=" << num_rounds << ' '
            << std::endl;

  scan(model, num_threads, num_rounds);

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
