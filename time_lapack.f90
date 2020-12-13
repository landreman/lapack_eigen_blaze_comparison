
program lapack

  implicit none

  integer :: N, N_matmuls, N_solves
  integer :: j, j1, j2, index
  double precision, allocatable :: m1(:,:), m2(:,:), m3(:,:), v1(:)
  double precision :: start_time, end_time
  character(len=64) :: arg
  
  ! Variables needed by BLAS DGEMM:
  character :: TRANSA, TRANSB
  integer :: M, K, LDA, LDB, LDC
  double precision :: BLAS_ALPHA=1, BLAS_BETA=0

  ! Variables needed by LAPACK:
  integer :: INFO
  integer, dimension(:), allocatable :: IPIV

  print "(a, i3)", "command_argument_count:", command_argument_count()
  call get_command_argument(1, arg)
  print "(a, a)", "get_command_argument(1, *): ", trim(arg)
  if (command_argument_count() .ne. 3) then
     stop "Usage: time_lapack N N_matmuls N_solves"
  end if

  read(arg, *) N
  call get_command_argument(2, arg)
  read(arg, *) N_matmuls
  call get_command_argument(3, arg)
  read(arg, *) N_solves

  print "(a,i5,a,i6,a,i6)", "Matrix size N:", N, ", N_matmuls:", N_matmuls, ", N_solves:", N_solves
  
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
  do j = 1, N_matmuls
     m1 = 1
     m2 = 2
     call DGEMM(TRANSA,TRANSB,M,N,K,BLAS_ALPHA,m1,LDA,m2,LDB,BLAS_BETA,m3,LDC)
  end do
  call cpu_time(end_time)

  print *, "Elapsed time for BLAS DGEMM:", end_time - start_time

  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  call cpu_time(start_time)
  do j = 1, N_solves
     v1 = 1
     ! Set the matrix to something dense and nonsingular
     index = -1
     do j1 = 1, N
        do j2 = 1, N
           m1(j2, j1) = j1 - (1.3d+0) * j2 + index
           index = index + 1
        end do
        m1(j1, j1) = j1
     end do
     !print *,"Matrix:"
     !do j1 = 1, N
     !   print *, m1(j1, :)
     !end do
     call DGESV(N, 1, m1, N, IPIV, v1, N, INFO)
     if (INFO /= 0) then
        print *, "Error in LAPACK call DGESV: info = ", INFO
        stop
     end if
  end do
  call cpu_time(end_time)

  print *, "Elapsed time for LAPACK DGESV:", end_time - start_time
  !print *, "Solution of linear system:"
  !print *, v1
  
end program lapack
