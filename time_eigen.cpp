#include <iostream>
#include <ctime>
#include <chrono>
#include <stdexcept>
#include <sstream>
#include <Eigen/Dense>

// For benchmark of the various linear solvers in Eigen see
// http://eigen.tuxfamily.org/dox/group__DenseDecompositionBenchmark.html

// Other useful pages:
// http://eigen.tuxfamily.org/dox/group__QuickRefPage.html
// http://eigen.tuxfamily.org/dox/group__TutorialMatrixClass.html
// http://eigen.tuxfamily.org/dox/group__TutorialLinearAlgebra.html

using namespace Eigen;

int main(int argc, char* argv[]) {
  std::cout << "Usage: time_eigen N N_matmuls N_solves" << std::endl;

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
  std::string algorithm;

  /////////////////////////////////////////////

  for (int j_alg = 0; j_alg < 4; j_alg++) {
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
      switch (j_alg) {
      case 0:
	v2 = m1.partialPivLu().solve(v1);
	algorithm = "partialPivLu";
	break;
      case 1:
	v2 = m1.householderQr().solve(v1);
	algorithm = "householderQr";
	break;
      case 2:
	v2 = m1.colPivHouseholderQr().solve(v1);
	algorithm = "colPivHouseholderQr";
	break;
      case 3:
	v2 = m1.completeOrthogonalDecomposition().solve(v1);
	algorithm = "completeOrthogonalDecomposition";
	break;
      default:
	throw std::runtime_error("Should not get here!");
      }
    }
    end_time = std::clock();
    end = std::chrono::steady_clock::now();
    elapsed = end - start;
    std::cout << "Time for Eigen solves from chrono with " << algorithm << ":           "
	      << elapsed.count() << std::endl;
    std::cout << "Time for Eigen solves from ctime with " << algorithm << " (CPU time): "
	      << double(end_time - start_time) / CLOCKS_PER_SEC << std::endl;
    // std::cout << "Solution of linear system:" << std::endl << v2 << std::endl;
  }
  
  return 0;
}
