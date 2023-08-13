#include "levelgraph.hpp"
#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>

std::chrono::microseconds measure_time_hpx(LevelGraph& graph, size_t num_threads) {
  auto beg = std::chrono::high_resolution_clock::now();

  for(size_t l=0; l<graph.level(); l++){
    std::vector<hpx::future<void>> tasks;
    for(size_t i=0; i<graph.length(); i++){
      tasks.push_back(hpx::async([&](){
        Node& n = graph.node_at(l, i);
        n.mark();
      }));
    }
    hpx::wait_all(tasks);
  }

  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(end - beg);
}

void reduce_sum(
  const std::string& model,
  const unsigned num_threads,
  const unsigned num_rounds
  ) {

  std::cout << std::setw(12) << "size"
            << std::setw(12) << "runtime"
            << std::endl;

  for(int i=1; i<=4510; i += 15) {

    double runtime {0.0};

    LevelGraph graph(i, i);

    for(unsigned j=0; j<num_rounds; ++j) {

     if(model == "hpx") {
        runtime += measure_time_hpx(graph, num_threads).count();
      }
      else assert(false);
      graph.clear_graph();
    }

    std::cout << std::setw(12) << graph.graph_size()
              << std::setw(12) << runtime / num_rounds / 1e3
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


