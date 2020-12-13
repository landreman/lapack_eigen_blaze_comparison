#include <iostream>
#include <ctime>
#include <chrono>
#include <Eigen/Dense>

// For benchmark of the various linear solvers in Eigen see
// http://eigen.tuxfamily.org/dox/group__DenseDecompositionBenchmark.html

// Other useful pages:
// http://eigen.tuxfamily.org/dox/group__QuickRefPage.html
// http://eigen.tuxfamily.org/dox/group__TutorialMatrixClass.html
// http://eigen.tuxfamily.org/dox/group__TutorialLinearAlgebra.html

using namespace Eigen;

int main(int argc, char* argv[]) {
  std::cout << "Hello world!" << std::endl;

  const int N = 51;
  const int N_matmuls = 1;
  const int N_solves = 20000;
  
  MatrixXd m1(N, N);
  MatrixXd m2(N, N);
  MatrixXd m3(N, N);
  std::time_t start_time, end_time;

  start_time = std::clock();
  auto start = std::chrono::steady_clock::now();
  for(int j = 1; j <= N_matmuls; j++) {
    m1 = MatrixXd::Constant(N, N, 1.0);
    m2 = MatrixXd::Constant(N, N, 2.0);
    m3 = m1 * m2;
  }
  end_time = std::clock();
  auto end = std::chrono::steady_clock::now();

  // std::cout << "Time for Eigen matmuls: " << end_time - start_time << std::endl;
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Time for Eigen matmuls from chrono:           "
	    << elapsed.count() << std::endl;
  std::cout << "Time for Eigen matmuls from ctime (CPU time): "
	    << double(end_time - start_time) / CLOCKS_PER_SEC << std::endl;
  // std::cout << m3 << std::endl;

  /////////////////////////////////////////////

  VectorXd v1(N);
  VectorXd v2(N);
  int index, j1, j2;
  start_time = std::clock();
  start = std::chrono::steady_clock::now();
  for(int j = 1; j <= N_solves; j++) {
    v1 = VectorXd::Constant(N, 1.0);
    index = -1;
    for(j1 = 0; j1 < N; j1++) {
      for(j2 = 0; j2 < N; j2++) {
	m1(j2, j1) = (j1 + 1) - 1.3 * (j2 + 1) + index;
	index++;
      }
      m1(j1, j1) = j1 + 1;
    }
    // std::cout << "Matrix:" << std::endl << m1 << std::endl;
    v2 = m1.partialPivLu().solve(v1);
  }
  end_time = std::clock();
  end = std::chrono::steady_clock::now();
  elapsed = end - start;
  std::cout << "Time for Eigen solves from chrono with partialPivLu:           "
	    << elapsed.count() << std::endl;
  std::cout << "Time for Eigen solves from ctime with partialPivLu (CPU time): "
	    << double(end_time - start_time) / CLOCKS_PER_SEC << std::endl;
  // std::cout << "Solution of linear system:" << std::endl << v2 << std::endl;
  
  /////////////////////////////////////////////

  start_time = std::clock();
  start = std::chrono::steady_clock::now();
  for(int j = 1; j <= N_solves; j++) {
    v1 = VectorXd::Constant(N, 1.0);
    index = -1;
    for(j1 = 0; j1 < N; j1++) {
      for(j2 = 0; j2 < N; j2++) {
	m1(j2, j1) = (j1 + 1) - 1.3 * (j2 + 1) + index;
	index++;
      }
      m1(j1, j1) = j1 + 1;
    }
    // std::cout << "Matrix:" << std::endl << m1 << std::endl;
    v2 = m1.colPivHouseholderQr().solve(v1);
  }
  end_time = std::clock();
  end = std::chrono::steady_clock::now();
  elapsed = end - start;
  std::cout << "Time for Eigen solves from chrono with colPivHouseholderQr:           "
	    << elapsed.count() << std::endl;
  std::cout << "Time for Eigen solves from ctime with colPivHouseholderQr (CPU time): "
	    << double(end_time - start_time) / CLOCKS_PER_SEC << std::endl;
  // std::cout << "Solution of linear system:" << std::endl << v2 << std::endl;
  
  return 0;
}
