#
# ./vxl/vnl/tests/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#
LINK_DIRECTORIES(
${allvxl_BINARY_DIR}/vxl/vnl 
${allvxl_BINARY_DIR}/vxl/vnl/algo 
${allvxl_BINARY_DIR}/v3p/netlib
)

LINK_LIBRARIES( 
vnl_algo 
netlib 
vnl 
vcl
)


EXECUTABLES(
test_fft1d 
test_fft2d 
test_math 
test_matrix 
test_matrix_fixed 
test_rnpoly_roots 
test_sparse_matrix 
test_qsvd 
test_sample 
test_functions 
test_numeric_limits 
test_rpoly_roots 
test_cpoly_roots 
test_complex 
test_determinant 
test_qr 
test_svd 
test_real_eigensystem 
test_complex_eigensystem 
test_symmetric_eigensystem 
test_generalized_eigensystem 
test_matrix_exp 
test_vector 
)
