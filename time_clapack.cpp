#include <iostream>
#include <ctime>
#include <chrono>
#include <stdexcept>
#include <sstream>
#include <valarray>

extern "C" {
  void dgemm_(char* TRANSA, char* TRANSB, int* M, int* N, int* K, double* ALPHA,
	      double* A, int* LDA, double* B, int* LDB, double* BETA, double* C, int* LDC);

  void dgesv_(int* N, int* NRHS, double* A, int* LDA, int* IPIV, double* B, int* LDB, int* INFO);
}

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
  
  std::valarray<double> m1(N * N);
  std::valarray<double> m2(N * N);
  std::valarray<double> m3(N * N);
  std::time_t start_time, end_time;
  char TRANSA = 'N', TRANSB = 'N';
  double ALPHA = 1.0, BETA = 0.0;
  
  start_time = std::clock();
  auto start = std::chrono::steady_clock::now();
  for(int j = 1; j <= N_matmuls; j++) {
    m1 = 1.0;
    m2 = 2.0;
    // m3 = m1 * m2;
    dgemm_(&TRANSA, &TRANSB, &N, &N, &N, &ALPHA, &m1[0], &N, &m2[0], &N, &BETA, &m3[0], &N);
  }
  end_time = std::clock();
  auto end = std::chrono::steady_clock::now();

  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Time for clapack matmuls from chrono:           "
	    << elapsed.count() << std::endl;
  std::cout << "Time for clapack matmuls from ctime (CPU time): "
	    << double(end_time - start_time) / CLOCKS_PER_SEC << std::endl;
  if (N <= 10) {
    std::cout << "Result of matmul:";
    for (int j = 0; j < N * N; j++) std::cout << " " << m3[j];
    std::cout << std::endl;
  }
  
  /////////////////////////////////////////////

  std::valarray<double> v1(N);
  int index, j1, j2;
  std::valarray<int> IPIV(N);
  int one = 1, INFO;

  /////////////////////////////////////////////

  start_time = std::clock();
  start = std::chrono::steady_clock::now();
  for(int j = 1; j <= N_solves; j++) {
    v1 = 1.0;
    index = -1;
    for(j1 = 0; j1 < N; j1++) {
      for(j2 = 0; j2 < N; j2++) {
	//m1(j2, j1) = (j1 + 1) - 1.3 * (j2 + 1) + index;
	m1[j2 + N * j1] = (j1 + 1) - 1.3 * (j2 + 1) + index;
	index++;
      }
      m1[j1 + N * j1] = j1 + 1;
    }
    // std::cout << "Matrix:" << std::endl << m1 << std::endl;
    dgesv_(&N, &one, &m1[0], &N, &IPIV[0], &v1[0], &N, &INFO);
  }
  end_time = std::clock();
  end = std::chrono::steady_clock::now();
  elapsed = end - start;
  std::cout << "Time for clapack solves from chrono:           "
	    << elapsed.count() << std::endl;
  std::cout << "Time for clapack solves from ctime (CPU time): "
	    << double(end_time - start_time) / CLOCKS_PER_SEC << std::endl;
  if (N <= 10) {
    std::cout << "Solution of linear system:";
    for (int j = 0; j < N; j++) std::cout << " " << v1[j];
    std::cout << std::endl;
  }
  
  return 0;
}
