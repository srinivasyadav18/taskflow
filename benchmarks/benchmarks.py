#!/usr/bin/env python3
import os
import subprocess
import sys
import math
import argparse
import matplotlib.pyplot as plt
import statistics as stat

base_dir="/work/srinivasyadav227/2023/sc23_tf/"

def analyze(Y):
  print(
    "  -> [min, max, avg] = [%f, %f, %f]" %
    (min(Y), max(Y), stat.mean(Y))
  )

###########################################################
# regression: run
###########################################################
def run(target, method, thread, round):

  exe = './bench_' + target
  tmp_file = base_dir + 'logs_poster/' + f'{target}_{method}.log'
  print('Writing to : ', tmp_file)

  if (method == 'hpx') or (method == 'hpx_simd'):
    with open(tmp_file, "w") as ofs:
      exe_list = [f'{exe}_hpx', f'--m={method}', f'--hpx:threads={str(thread)}', f'--t={str(thread)}', f'--r={str(round)}']
      print(f'Executing: {exe_list}')
      subprocess.call([f'{exe}_hpx', f'--m={method}', f'--hpx:threads={str(thread)}', f'--t={str(thread)}', f'--r={str(round)}'],
                       stdout=ofs)
  else:
    with open(tmp_file, "w") as ofs:
      exe_list = [exe, '-m', method, '-t', str(thread), '-r', str(round)],
      print(f'Executing: {exe_list}')
      subprocess.call([exe, '-m', method, '-t', str(thread), '-r', str(round)], stdout=ofs)

  X = []
  Y = []

  with open(tmp_file, "r") as ifs:
    ifs.readline()
    ifs.readline()
    for line in ifs:
      token = line.split()
      assert len(token) == 2, "output line must have exactly two numbers"
      X.append(int(token[0]))
      Y.append(float(token[1]))

  #analyze(Y)

  return X, Y

###########################################################
# main function
###########################################################
def main():

  # example usage
  # -b wavefront graph_traversal -t 1 2 3 -m tbb omp tf

  parser = argparse.ArgumentParser(description='regression')

  parser.add_argument(
    '-b', '--benchmarks',
    nargs='+',
    help='list of benchmark names',
    choices=['wavefront',
             'graph_traversal',
             'binary_tree',
             'linear_chain',
             'matrix_multiplication',
             'mnist',
             'sort',
             'for_each',
             'reduce_sum',
             'scan',
             'black_scholes',
             'mandelbrot'
             ],
    required=True
  )

  benchmark_titles = {}
  benchmark_titles['for_each'] = ['Number of Elements', 'for each']
  benchmark_titles['scan'] = ['Number of Elements', 'Inclusive Scan']
  benchmark_titles['sort'] = ['Number of Elements', 'Sort']
  benchmark_titles['mandelbrot'] = ['Number of Elements', 'Mandelbrot Set']
  benchmark_titles['reduce_sum'] = ['Number of Elements', 'Reduce Sum']
  benchmark_titles['matrix_multiplication'] = ['Matrix size (m = n = o)', 'Matrix Multiplication']

  parser.add_argument(
    '-m','--methods',
    nargs='+',
    help='list of tasking methods',
    default=['seq', 'tf', 'tbb', 'omp', 'hpx'],
    choices=['seq', 'tf', 'tbb', 'omp', 'hpx', 'hpx_simd']
  )

  parser.add_argument(
    '-r', '--num_rounds',
    type=int,
    help='number of rounds to average',
    default=1
  )

  # parse the arguments
  args = parser.parse_args()

  print('benchmarks: ', args.benchmarks)
  print('methods:', args.methods)
  print('num_rounds:', args.num_rounds)
  print()

  for benchmark in args.benchmarks:
    fig, ax = plt.subplots(figsize=(10,6))
    for method in args.methods:
      print(benchmark, method)
      X, Y = run(
        benchmark, method, 40, args.num_rounds
      )
      if method == 'tf':
        ls = '--'
        marker = 'o'
        # color  = 'b'
      elif method == 'omp':
        ls = '--'
        marker = 'D'
        # color  = 'g'
      elif method == 'hpx':
        ls = '-.'
        marker = '>'
      elif method == 'hpx_simd':
        ls = '-.'
        marker = '<'
        # color  = 'm'
      elif method == 'tbb':
        ls = '--'
        marker = '+'
      else:
        ls = ':'
        marker = 's'
        # color  = 'r'

      print('X:',X)
      print('Y:',Y)
      plt.plot(X, Y, label=method, linestyle=ls, marker=marker)
      print('------------------------------------------------\n')
    plt.title(f'{benchmark_titles[benchmark][1]} Benchmark\n INTEL XEON 8358, Rostam Cluster, CCT, LSU', fontsize=20)
    plt.xscale('log', base=10)
    plt.yscale('log', base=10)
    # plt.yticks(ticks=[10, 100, 1000], labels=[10, 100, 1000], fontsize=16)
    # plt.xticks(ticks=[1, 2, 4, 8, 16, 32, 64, 128], labels=[1, 2, 4, 8, 16, 32, 64, 128], fontsize=16)
    plt.xlabel(benchmark_titles[benchmark][0], fontweight="heavy", fontsize=18)
    plt.ylabel("Runtime (s) in log scale", fontweight="heavy", fontsize=18)
    plt.legend(loc='upper left', fontsize=18)
    plt.show()
    fig_path = f'{base_dir}plots_poster/poster_{benchmark}.png'
    print('Generated plot:', fig_path)
    plt.savefig(fig_path)

# run the main entry
if __name__ == "__main__":
  main()



