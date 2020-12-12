#include <iostream>
#include <ctime>
#include <chrono>
#include <Eigen/Dense>

using namespace Eigen;

int main(int argc, char* argv[]) {
  std::cout << "Hello world!" << std::endl;

  const int N = 51;

  MatrixXd m1(N, N);
  MatrixXd m2(N, N);
  MatrixXd m3(N, N);
  std::time_t start_time, end_time;

  start_time = std::clock();
  auto start = std::chrono::steady_clock::now();
  for(int j = 1; j <= 100000; j++) {
    m1 = MatrixXd::Constant(N, N, 1.0);
    m2 = MatrixXd::Constant(N, N, 2.0);
    m3 = m1 * m2;
  }
  // std::cout << m3 << std::endl;
  end_time = std::clock();
  auto end = std::chrono::steady_clock::now();

  // std::cout << "Time for Eigen matmuls: " << end_time - start_time << std::endl;
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Time for Eigen matmuls from chrono:           "
	    << elapsed.count() << std::endl;
  std::cout << "Time for Eigen matmuls from ctime (CPU time): "
	    << double(end_time - start_time) / CLOCKS_PER_SEC << std::endl;
  
  return 0;
}
