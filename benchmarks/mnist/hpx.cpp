#include "dnn.hpp"
#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>

// Function: measure_time_hpx
std::chrono::milliseconds measure_time_hpx(
  unsigned num_epochs,
  unsigned num_threads
) {
  auto dnn {build_dnn(num_epochs)};
  auto t1 = std::chrono::high_resolution_clock::now();
  run_hpx(dnn, num_threads);
  auto t2 = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
}

// Procedure
void mnist(
  const std::string& model,
  const unsigned min_epochs,
  const unsigned max_epochs,
  const unsigned num_threads,
  const unsigned num_rounds
) {

  std::cout << std::setw(12) << "epochs"
            << std::setw(12) << "runtime"
            << std::endl;

  for(unsigned epochs=min_epochs; epochs <= max_epochs; epochs += 10) {

    double runtime  {0.0};

    for(unsigned i=0; i<num_rounds; i++) {

      if(model == "hpx") {
        runtime += measure_time_hpx(epochs, num_threads).count();
      }
      else assert(false);

      std::cout << std::setw(12) << epochs
                << std::setw(12) << runtime / num_rounds / 1e3
                << std::endl;
    }
  }

}

int hpx_main(hpx::program_options::variables_map& vm) {

  std::string model = vm["m"].as<std::string>();
  std::size_t num_threads = vm["t"].as<std::size_t>();
  std::size_t num_rounds = vm["r"].as<std::size_t>();
  std::size_t max_epochs = vm["E"].as<std::size_t>();
  std::size_t min_epochs = vm["e"].as<std::size_t>();

  std::cout << "model=" << model << ' '
            << "num_threads=" << num_threads << ' '
            << "num_rounds=" << num_rounds << ' '
            << "min_epochs=" << min_epochs << ' '
            << "max_epochs=" << max_epochs << ' '
            << std::endl;

  mnist(model, min_epochs, max_epochs, num_threads, num_rounds);

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
        "Number of rounds"),
        ("e",
        hpx::program_options::value<std::size_t>()->default_value(10),
        "Min number of epochs"),
        ("E",
        hpx::program_options::value<std::size_t>()->default_value(100),
        "Max number of epochs"), 

    // clang-format on
    hpx::init_params init_args;
    init_args.desc_cmdline = cmdline;

    return hpx::init(hpx_main, argc, argv, init_args);
}


