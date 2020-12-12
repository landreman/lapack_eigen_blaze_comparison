
program lapack

  implicit none

  integer :: N = 51
  integer :: j, j1, j2, index
  double precision, allocatable :: m1(:,:), m2(:,:), m3(:,:), v1(:)
  double precision :: start_time, end_time

  ! Variables needed by BLAS DGEMM:
  character :: TRANSA, TRANSB
  integer :: M, K, LDA, LDB, LDC
  double precision :: BLAS_ALPHA=1, BLAS_BETA=0

  ! Variables needed by LAPACK:
  integer :: INFO
  integer, dimension(:), allocatable :: IPIV
  
  allocate(m1(N, N))
  allocate(m2(N, N))
  allocate(m3(N, N))

  allocate(v1(N))
  allocate(IPIV(N))
  
  TRANSA = 'N' ! No transposes
  TRANSB = 'N'
  M = N
  K = N
  LDA = N
  LDB = N
  LDC = N
  call cpu_time(start_time)
  do j = 1, 100000
     m1 = 1
     m2 = 2
     call DGEMM(TRANSA,TRANSB,M,N,K,BLAS_ALPHA,m1,LDA,m2,LDB,BLAS_BETA,m3,LDC)
  end do
  call cpu_time(end_time)

  print *, "Elapsed time for BLAS DGEMM:", end_time - start_time

  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  call cpu_time(start_time)
  do j = 1, 0
     v1 = 1
     ! Set the matrix to something dense and nonsingular
     index = -1
     do j1 = 1, N
        do j2 = 1, N
           m1(j2, j1) = j1 - 1.3 * j2 + index
           index = index + 1
        end do
     end do
     call DGESV(N, 1, m1, N, IPIV, v1, N, INFO)
     if (INFO /= 0) then
        print *, "Error in LAPACK call DGESV: info = ", INFO
        stop
     end if
  end do
  call cpu_time(end_time)

  print *, "Elapsed time for LAPACK DGESV:", end_time - start_time

  
end program lapack
