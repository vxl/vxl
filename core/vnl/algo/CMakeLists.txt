# ./vxl/vnl/algo/CMakeLists.txt

ADD_LIBRARY( vnl_algo vnl_algo_sources)

SOURCE_FILES( vnl_algo_sources
  vnl_cholesky.cxx
  vnl_real_eigensystem.cxx
  vnl_complex_eigensystem.cxx
  vnl_symmetric_eigensystem.cxx
  vnl_generalized_eigensystem.cxx
  vnl_sparse_symmetric_eigensystem.cxx
  vnl_generalized_schur.cxx
  vnl_discrete_diff.cxx
  vnl_levenberg_marquardt.cxx
  vnl_conjugate_gradient.cxx
  vnl_lbfgs.cxx
  vnl_amoeba.cxx
  vnl_powell.cxx
  vnl_brent.cxx
  vnl_lsqr.cxx
  vnl_rpoly_roots.cxx
  vnl_cpoly_roots.cxx
  vnl_rnpoly_solve.cxx
  vnl_fft.cxx
  vnl_chi_squared.cxx
  vnl_gaussian_kernel_1d.cxx
)

AUX_SOURCE_DIRECTORY(Templates vnl_algo_sources)
INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/v3p/netlib)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/vxl/vnl/algo/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)
