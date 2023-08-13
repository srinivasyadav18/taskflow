#include "mandel.hpp"
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>

int H = 1000;
int W = 1000;
unsigned char* RGB = nullptr;

// matrix_multiplication_hpx
void mandelbrot_hpx(unsigned num_threads, int d = D) {
  hpx::experimental::for_loop(hpx::execution::par, 0, H, [&](auto i){
    for(int j=0; j<W; j++) {
      auto xy = scale_xy(i, j);
      auto value = escape_time(xy.first, xy.second, d);
      auto k = 3 * ( j * W + i );
      std::tie(RGB[k], RGB[k+1], RGB[k+2]) = get_color(value);
    }
  });
}

std::chrono::nanoseconds measure_time_hpx(unsigned num_threads) {
  auto beg = std::chrono::high_resolution_clock::now();
  mandelbrot_hpx(num_threads);
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - beg);
}

void mandelbrot(
  const std::string& model,
  const unsigned num_threads,
  const unsigned num_rounds
  ) {

  std::cout << std::setw(12) << "size"
            << std::setw(12) << "runtime"
            << std::endl;

  for(int N = 100; N<=1000; N+=100) {

    W = N;
    H = N;

    double runtime {0.0};
    RGB = static_cast<unsigned char *>(malloc (W * H * 3 * sizeof(unsigned char)));

    for(unsigned j=0; j<num_rounds; ++j) {
     if(model == "hpx") {
        runtime += measure_time_hpx(num_threads).count();
      }
      else assert(false);
    }

    std::cout << std::setw(12) << N
              << std::setw(12) << runtime / num_rounds / 1e9
              << std::endl;
    free(RGB);
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
  
  mandelbrot(model, num_threads, num_rounds);

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