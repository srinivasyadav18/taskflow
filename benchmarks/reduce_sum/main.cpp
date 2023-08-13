#include "reduce_sum.hpp"
#include <CLI11.hpp>

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

      if(model == "tf") {
        runtime += measure_time_taskflow(num_threads).count();
      }
      else if(model == "tbb") {
        runtime += measure_time_tbb(num_threads).count();
      }
      else if(model == "omp") {
        runtime += measure_time_omp(num_threads).count();
      }
      else if(model == "seq") {
        runtime += measure_time_seq(num_threads).count();
      }
      else assert(false);
    }

    std::cout << std::setw(12) << N
              << std::setw(12) << runtime / num_rounds / 1e9
              << std::endl;
  }
}

int main(int argc, char* argv[]) {

  CLI::App app{"Reduce Sum"};

  unsigned num_threads {1};
  app.add_option("-t,--num_threads", num_threads, "number of threads (default=1)");

  unsigned num_rounds {1};
  app.add_option("-r,--num_rounds", num_rounds, "number of rounds (default=1)");

  std::string model = "tf";
  app.add_option("-m,--model", model, "model name tbb|omp|tf (default=tf)")
     ->check([] (const std::string& m) {
        if(m != "tbb" && m != "tf" && m != "omp" && m != "seq") {
          return "model name should be seq or \"tbb\", \"omp\", or \"tf\"";
        }
        return "";
     });

  CLI11_PARSE(app, argc, argv);

  std::cout << "model=" << model << ' '
            << "num_threads=" << num_threads << ' '
            << "num_rounds=" << num_rounds << ' '
            << std::endl;

  reduce_sum(model, num_threads, num_rounds);

  return 0;
}


