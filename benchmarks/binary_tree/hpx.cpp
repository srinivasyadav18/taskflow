#include "binary_tree.hpp"
#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>

// binary_tree_hpx
void binary_tree_hpx(size_t num_layers, unsigned num_threads) {

  std::atomic<size_t> counter {0};

  std::vector<hpx::shared_future<void>> tasks;
  const std::size_t N = 1 << num_layers;
  tasks.reserve(N);

  for(unsigned i=1; i<N; i++) {
    tasks.emplace_back(hpx::async([&](){
      counter.fetch_add(1, std::memory_order_relaxed);
    }.share()));
  }

  for(unsigned i=1; i<N; i++) {
    unsigned l = i << 1;
    unsigned r = l + 1;
    if(l < N && r < N) {
      tasks[i].then([](hpx::launch::async, ), tasks[r]);
      tasks[i].then(hpx::launch::async, [](auto&& fA) { fA.get(); });
    }
  }

  executor.run(taskflow).get();
  assert(counter + 1 == tasks.size());
}

std::chrono::microseconds measure_time_hpx(
  size_t num_layers,
  unsigned num_threads
) {
  auto beg = std::chrono::high_resolution_clock::now();
  binary_tree_hpx(num_layers, num_threads);
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(end - beg);
}

void binary_tree(
  const std::string& model,
  const size_t num_layers,
  const unsigned num_threads,
  const unsigned num_rounds
  ) {

  std::cout << std::setw(12) << "size"
            << std::setw(12) << "runtime"
            << std::endl;

  for(size_t i=1; i<=num_layers; ++i) {

    double runtime {0.0};

    for(unsigned j=0; j<num_rounds; ++j) {
     if(model == "hpx") {
        runtime += measure_time_hpx(num_threads, num_layers).count();
      }
      else assert(false);
    }

    std::cout << std::setw(12) << N
              << std::setw(12) << runtime / num_rounds / 1e3
              << std::endl;
  }
}

int hpx_main(hpx::program_options::variables_map& vm) {

  std::string model = vm["m"].as<std::string>();
  std::size_t num_threads = vm["t"].as<std::size_t>();
  std::size_t num_rounds = vm["r"].as<std::size_t>();
  std::size_t num_layers = vm["l"].as<std::size_t>();

  std::cout << "model=" <<  model  << ' '
            << "num_threads=" << num_threads  << ' '
            << "num_rounds=" << num_rounds << ' '
            << std::endl;

  binary_tree(model, num_layers, num_threads, num_rounds);
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
        "Number of rounds")
        ("l",
        hpx::program_options::value<std::size_t>()->default_value(25),
        "Number of layers");
    // clang-format on
    hpx::init_params init_args;
    init_args.desc_cmdline = cmdline;

    return hpx::init(hpx_main, argc, argv, init_args);
}


