#
# ./vxl/vnl/algo/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

ADD_LIBRARY( vnl_algo vnl_algo_sources)

SOURCE_FILES( vnl_algo_sources
vnl_cholesky
vnl_real_eigensystem
vnl_complex_eigensystem
vnl_symmetric_eigensystem
vnl_generalized_eigensystem
vnl_sparse_symmetric_eigensystem
vnl_discrete_diff
vnl_levenberg_marquardt
vnl_conjugate_gradient
vnl_lbfgs
vnl_amoeba
vnl_powell
vnl_brent
vnl_lsqr
vnl_rpoly_roots
vnl_cpoly_roots
vnl_rnpoly_solve
vnl_fft
vnl_fftxd_prime_factors
vnl_determinant
vnl_chi_squared
vnl_gaussian_kernel_1d
)

AUX_SOURCE_DIRECTORY(Templates vnl_algo_sources)

