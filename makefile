ifdef NERSC_HOST
  HOSTNAME = $(NERSC_HOST)
else
  HOSTNAME?="laptop"
endif

ifdef MACHINE
  HOSTNAME = $(MACHINE)
endif

ifeq ($(HOSTNAME),cori)
  MY_HOST = cori
  FC = ftn
  ## NERSC documentation recommends against specifying -O3
  ## -mkl MUST APPEAR AT THE END!!
  EXTRA_COMPILE_FLAGS =  -mkl
  EXTRA_LINK_FLAGS =   -mkl -Wl,-ydgemm_
  # Above, the link flag "-Wl,-ydgemm_" causes the linker to report which version of DGEMM (the BLAS3 matrix-matrix-multiplication subroutine) is used.

else ifeq ($(HOSTNAME),pppl_gcc)
  MY_HOST=pppl_gcc
  NETCDF_F = $(NETCDF_FORTRAN_HOME)
  NETCDF_C = $(NETCDF_C_HOME)
  FC = mpifort
  EXTRA_COMPILE_FLAGS = -O2 -ffree-line-length-none -fexternal-blas  -I$(NETCDF_F)/include -I$(NETCDF_C)/include
  EXTRA_LINK_FLAGS =  -lopenblas -L$(NETCDF_C)/lib -lnetcdf -L$(NETCDF_F)/lib -lnetcdff

else ifeq ($(HOSTNAME),pppl_intel)
  MY_HOST=pppl_intel
  FC = mpifort
  NETCDF_F = $(NETCDF_FORTRAN_HOME)
  NETCDF_C = $(NETCDF_C_HOME)
  EXTRA_COMPILE_FLAGS =-mcmodel=large -O3 -m64 -unroll0 -fno-alias -ip -traceback \
    -Wl,--end-group \
     \
    -lpthread \
    -I$(NETCDF_F)/include -I$(NETCDF_C)/include \
    -mkl	
  EXTRA_LINK_FLAGS =  -mkl -L$(NETCDF_C)/lib -lnetcdf -L$(NETCDF_F)/lib -lnetcdff

else ifeq ($(CLUSTER),DRACO)
  MY_HOST=draco
  FC = mpiifort
  #EXTRA_COMPILE_FLAGS =  -mkl -I${NETCDF_HOME}/include
  #EXTRA_LINK_FLAGS =   -mkl -Wl,-ydgemm_ -L${NETCDF_HOME}/lib -lnetcdf -lnetcdff
  EXTRA_COMPILE_FLAGS =   -I${MKLROOT}/include -I${NETCDF_HOME}/include
  EXTRA_LINK_FLAGS =    -Wl,--start-group ${MKLROOT}/lib/intel64/libmkl_intel_lp64.a ${MKLROOT}/lib/intel64/libmkl_intel_thread.a ${MKLROOT}/lib/intel64/libmkl_core.a -Wl,--end-group -liomp5 -lpthread -lm -ldl -Wl,-ydgemm_ -L${NETCDF_HOME}/lib -lnetcdf -lnetcdff

else
  MY_HOST=macports
  #FC = gfortran-mp-8
  #CXX = g++-mp-8
  FC = mpif90-mpich-gcc8
  CXX = mpicxx-mpich-gcc8
  EXTRA_F_COMPILE_FLAGS = -ffree-line-length-none -O2
  EXTRA_F_LINK_FLAGS = -framework Accelerate
  EXTRA_C_COMPILE_FLAGS = -O2
endif


.PHONY: all clean

all: lapack eigen_blaze

lapack.o: lapack.f90
	$(FC) $(EXTRA_F_COMPILE_FLAGS) -c $<

lapack: lapack.o
	$(FC) -o lapack lapack.o $(EXTRA_F_LINK_FLAGS)

eigen_blaze.o: eigen_blaze.cpp
	$(CXX) $(EXTRA_C_COMPILE_FLAGS) -I eigen -c $<

eigen_blaze: eigen_blaze.o
	$(CXX) -o eigen_blaze eigen_blaze.o $(EXTRA_C_LINK_FLAGS)

clean:
	rm -f *.o *.mod *.MOD *~ lapack eigen_blaze *.a

test_make:
	@echo MY_HOST is $(MY_HOST)
	@echo HOSTNAME is $(HOSTNAME)
	@echo MACHINE is $(MACHINE)
	@echo FC is $(FC)
	@echo CXX is $(CXX)
	@echo EXTRA_F_COMPILE_FLAGS is $(EXTRA_F_COMPILE_FLAGS)
	@echo EXTRA_F_LINK_FLAGS is $(EXTRA_F_LINK_FLAGS)
	@echo EXTRA_C_COMPILE_FLAGS is $(EXTRA_C_COMPILE_FLAGS)
	@echo EXTRA_C_LINK_FLAGS is $(EXTRA_C_LINK_FLAGS)
