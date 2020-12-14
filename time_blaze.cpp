#include <iostream>
#include <ctime>
#include <chrono>
#include <blaze/Math.h>

using namespace blaze;

int main(int argc, char* argv[]) {
  std::cout << "Usage: time_eigen N N_matmuls N_solves" << std::endl;
  std::cout.precision(15);

  if (argc != 4) {
    throw std::runtime_error("The number of arguments must be 3.");
  }

  // Parse the command-line arguments:
  std::stringstream arg1(argv[1]), arg2(argv[2]), arg3(argv[3]);
  int N, N_matmuls, N_solves;
  arg1 >> N;
  arg2 >> N_matmuls;
  arg3 >> N_solves;
  std::cout << "Matrix size N: " << N << ", N_matmuls: " << N_matmuls <<
    ", N_solves: " << N_solves << std::endl;

  DynamicMatrix<double> m1(N, N);
  DynamicMatrix<double> m2(N, N);
  DynamicMatrix<double> m3(N, N);
  std::time_t start_time, end_time;
  
  start_time = std::clock();
  auto start = std::chrono::steady_clock::now();
  for(int j = 1; j <= N_matmuls; j++) {
    m1 = 1.0;
    m2 = 2.0;
    m3 = m1 * m2;
  }
  end_time = std::clock();
  auto end = std::chrono::steady_clock::now();

  // std::cout << "Time for Blaze matmuls: " << end_time - start_time << std::endl;
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Time for Blaze matmuls from chrono:           "
	    << elapsed.count() << std::endl;
  std::cout << "Time for Blaze matmuls from ctime (CPU time): "
	    << double(end_time - start_time) / CLOCKS_PER_SEC << std::endl;
  // std::cout << m3 << std::endl;

  /////////////////////////////////////////////
  
  /////////////////////////////////////////////

  DynamicVector<double> v1(N);
  DynamicVector<double> v2(N);
  int index, j1, j2;

  start_time = std::clock();
  start = std::chrono::steady_clock::now();
  for(int j = 1; j <= N_solves; j++) {
    v1 = 1.0;
    index = -1;
    for(j1 = 0; j1 < N; j1++) {
      for(j2 = 0; j2 < N; j2++) {
	m1(j2, j1) = (j1 + 1) - 1.3 * (j2 + 1) + index;
	index++;
      }
      m1(j1, j1) = j1 + 1;
    }
    // std::cout << "Matrix:" << std::endl << m1 << std::endl;
    
    v2 = solve(m1, v1);
  }
  
  end_time = std::clock();
  end = std::chrono::steady_clock::now();
  elapsed = end - start;
  std::cout << "Time for Blaze solves from chrono:           "
	    << elapsed.count() << std::endl;
  std::cout << "Time for Blaze solves from ctime (CPU time): "
	    << double(end_time - start_time) / CLOCKS_PER_SEC << std::endl;
  if (N <= 10) {
    std::cout << "Solution of linear system:" << std::endl << v2 << std::endl;
  }
  
  return 0;
}
